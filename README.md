### trying to make it minimal
Dependencies
* OpenGL (+ GLAD + glm)
* glfw
* Dear ImGui
* MinimalSocket (low-weight abstraction)
* nlohmann::json

## UDP Connections
As described in https://kno.wled.ge/interfaces/udp-realtime/, you can make your WLED send UDP packages
that update each LED. This works via the configurable port and might need:
https://www.techopedia.com/definition/4961/administrative-privileges