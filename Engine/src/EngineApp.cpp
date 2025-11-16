#include <Codi.h>
#include <Codi/Core/EntryPoint.h>

namespace Codi {

    class EngineApp : public Application {
    public:
        EngineApp(const ApplicationSpecification& spec) : Application(spec) {}
        ~EngineApp() {}
    };

    Application* CreateApplication() {
        ApplicationSpecification spec;
        spec.Name = "Codi Engine";
        spec.Width = 1280;
        spec.Height = 720;

        return new EngineApp(spec);
    }

} // namespace Codi
