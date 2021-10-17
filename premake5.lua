workspace "raytracer-cpp"
    configurations { "debug", "release" }

project "raytracer-cpp"
   kind "ConsoleApp"
   language "C++"

   symbols 'On'
  

   
   files { "**.h", "**.c", "**.cpp" }
   includedirs {
       "lib",
       "lib/glm"
    }

    optimize "Speed"

