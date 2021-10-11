workspace "raytracer-cpp"
    configurations { "debug", "release" }

project "raytracer-cpp"
   kind "ConsoleApp"
   language "C++"

   symbols 'On'
   warnings "Extra"

   
   files { "**.h", "**.c", "**.cpp" }
   includedirs {
       "lib",
       "lib/glm"
    }

    optimize "Speed"

