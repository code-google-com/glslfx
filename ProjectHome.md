# glslfx #

Supports a format similar to HLSL .fx when techniques and passes are defined. It loads the source from the files defined in the effect and adds support for #include in the source files.

Built to support OpenGL 3+ firsthand, but it is possible to use with older versions.

```
technique sample {
  pass foo {
    vertex: sample-vert.glsl
    geometry: sample-geom.glsl
    fragment: sample-frag.glsl
  }
}
```

# Features #
  * Loading of .glslfx-files
    * Has similarities with Nvidia-CG and HLSL-effects.
  * Allows to use the #include preprocessor directive.
    * Adds #line directives
  * Parses the infolog provided by the OpenGL driver
    * Reports both file and line on which the error/warning occured
    * Allows outputting errors/warnings in both gcc- and MSVC-style
  * Validator-tool to parse and check validity of an effect.
    * Usable from a buildsystem
  * C++ only