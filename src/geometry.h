#pragma once
#include "primitives/workers.h"
#include "primitives/producer_consumer.h"
#include "primitives/vector.h"
#include "common.h"

constexpr int vec3d_size = 3;
typedef float vec3d[vec3d_size];

class Print;
class HashedWord;

// Stored definition of Base Stations
struct BaseStationGeometryDef {
    float mat[9];    // Normalized rotation matrix.
    vec3d origin; // Origin point

    void print_def(uint32_t idx, Print &stream);
    bool parse_def(uint32_t idx, HashedWord *input_words, Print &err_stream);
};

struct SensorLocalGeometry {
    uint32_t input_idx;
    vec3d pos;  // Position of the sensor relative to the object.
};

// Stored definition of GeometryBuilder
struct GeometryBuilderDef {
    Vector<SensorLocalGeometry, 4> sensors;

    void print_def(uint32_t idx, Print &stream);
    bool parse_def(uint32_t idx, HashedWord *input_words, Print &err_stream);
};

// Parent, abstract class for GeometryBuilders.
class GeometryBuilder
    : public WorkerNode
    , public Consumer<SensorAnglesFrame>
    , public Producer<ObjectGeometry>  {
public:
    GeometryBuilder(uint32_t idx, const GeometryBuilderDef &geo_def,
                    const Vector<BaseStationGeometryDef, num_base_stations> &base_stations);
    
protected:
    uint32_t geo_builder_idx_;
    const Vector<BaseStationGeometryDef, num_base_stations> &base_stations_;
    GeometryBuilderDef def_;
};

// Simple class for single-point sensors.
class PointGeometryBuilder : public GeometryBuilder {
public:
    PointGeometryBuilder(uint32_t idx, const GeometryBuilderDef &geo_def,
                         const Vector<BaseStationGeometryDef, num_base_stations> &base_stations);
    virtual void consume(const SensorAnglesFrame& f);
    virtual void do_work(Timestamp cur_time);

    virtual bool debug_cmd(HashedWord *input_words);
    virtual void debug_print(Print& stream);

private:
    Timestamp last_success_;
};

// Helper node to convert coordinates to a different coordinate system.
class CoordinateSystemConverter
    : public WorkerNode
    , public Consumer<ObjectGeometry>
    , public Producer<ObjectGeometry> {
public:
    CoordinateSystemConverter(float m[9]);

    // Convert from standard Vive coordinate system to NED.
    // Needs angle between North and X axis, in degrees.
    static std::unique_ptr<CoordinateSystemConverter> NED(float angle_in_degrees);

    virtual void consume(const ObjectGeometry& geo);
    virtual bool debug_cmd(HashedWord *input_words);
    virtual void debug_print(Print& stream);

private:
    float mat_[9];
};