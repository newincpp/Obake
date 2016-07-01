package main

type ObjectJSON struct {
	Binary    BinaryJSON    `json:"Binary"`
	StaticLib StaticLibJSON `json:"StaticLib"`
	Plugin    PluginJSON    `json:"Plugin"`
	Builder   BuilderJSON   `json:"Builder"`
}

type BinaryJSON struct {
	Name           string   `json:"name"`
	StaticLibs     []string `json:"staticLibs"`
	Plugins        []string `json:"plugins"`
	SrcFolders     []string `json:"srcFolders"`
	SrcExtension   string   `json:"srcExtension"`
	HeadersFolders []string `json:"headersFolders"`
	ExternIncludes []string `json:"externIncludes"`
	OutFolder      string   `json:"outFolder"`
}

type StaticLibJSON struct {
	Name           string   `json:"name"`
	StaticLibs     []string `json:"staticLibs"`
	SrcFolders     []string `json:"srcFolders"`
	SrcExtension   string   `json:"srcExtension"`
	HeadersFolders []string `json:"headersFolders"`
	ExternIncludes []string `json:"externIncludes"`
	OutFolder      string   `json:"outFolder"`
}

type PluginJSON struct {
	Name           string   `json:"name"`
	StaticLibs     []string `json:"staticLibs"`
	SrcFolders     []string `json:"srcFolders"`
	SrcExtension   string   `json:"srcExtension"`
	HeadersFolders []string `json:"headersFolders"`
	ExternIncludes []string `json:"externIncludes"`
	OutFolder      string   `json:"outFolder"`
}

type BuilderJSON struct {
	Os            string   `json:"OS"`
	Toolchain     string   `json:"toolchain"`
	Binary        string   `json:"binary"`
	StaticLibs    []string `json:"staticLibs"`
	Plugins       []string `json:"plugins"`
	PluginsFolder string   `json:"pluginsFolder"`
	OutFolder     string   `json:"outFolder"`
	CompilerFlags []string `json:"compilerFlags"`
}