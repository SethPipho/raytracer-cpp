workspace "raytracer-cpp"
    configurations { "debug", "release" }

project "raytracer-cpp"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"

   symbols 'On'

   files { "src/**.h", "src/**.c", "src/**.cpp" }
   includedirs {
       "lib",
       "src"
    }

    optimize "Speed"
  