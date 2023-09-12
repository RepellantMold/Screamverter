-- premake5.lua
workspace "Screamverter"
   configurations { "Debug", "Release" }

project "Screamverter"
   kind "ConsoleApp"
   language "C"
   targetdir "bin/%{cfg.buildcfg}"

   files { "**.h", "**.c" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      optimize "Debug"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "Size"