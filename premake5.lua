workspace "raytracer-cpp"
    configurations { "debug", "release" }

project "raytracer-cpp"
   kind "ConsoleApp"
   language "C++"

   symbols 'On'

   files { "src/**.h", "src/**.c", "src/**.cpp" }
   includedirs {
       "lib",
       "src"
    }

    optimize "Speed"
    buildoptions { "-std=c++17" }

