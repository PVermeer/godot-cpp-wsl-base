#include "gdexample.h"
#include "ostream"

using namespace godot;

void GDExample::_register_methods()
{
    register_method("_process", &GDExample::_process);

    // Export properties
    register_property<GDExample, float>("amplitude", &GDExample::amplitude, 10.0);
    register_property<GDExample, float>("speed", &GDExample::set_speed, &GDExample::get_speed, 1.0);

    // Signals
    register_signal<GDExample>((char *)"position_changed", "node", GODOT_VARIANT_TYPE_OBJECT, "new_pos", GODOT_VARIANT_TYPE_VECTOR2);
}

GDExample::GDExample()
{
    // Constructor
}

GDExample::~GDExample()
{
    // Destructor
}

void GDExample::_init()
{
    // initialize any variables here
    time_passed = 0.0;
    time_emit = 0.0;
    amplitude = 10.0;
    speed = 1.0;
}

void GDExample::_process(float delta)
{
    time_passed += speed * delta;

    Vector2 new_position = Vector2(
        amplitude + (amplitude * sin(time_passed * 2.0)),
        amplitude + (amplitude * cos(time_passed * 1.5)));

    set_position(new_position);

    time_emit += delta;
    if (time_emit > 1.0)
    {
        emit_signal("position_changed", this, new_position);

        time_emit = 0.0;

        std::cout << "Delta: " << delta << "\n"
                  << "Time passed: " << time_passed << "\n";
    }
}

void GDExample::set_speed(float p_speed)
{
    speed = p_speed;
}
float GDExample::get_speed()
{
    return speed;
}
