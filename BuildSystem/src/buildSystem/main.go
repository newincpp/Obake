package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"strings"
)

type ObakeOSType int64
type ObakeBuildType int64

const obakeBSFilename string = "ObakeBuild.json"

const (
	BINARY ObakeBuildType = iota
	STATIC_LIB
	PLUGIN
	Builder
	NONE
)

const (
	WINDOWS ObakeOSType = iota
	LINUX
	OSX
	UNKNOWN
)

const (
	WINDOWS_STATIC_EXT  = ".lib"
	WINDOWS_DYNAMIC_EXT = ".dll"
	WINDOWS_BINARY_EXT  = ".exe"

	LINUX_STATIC_EXT  = ".a"
	LINUX_DYNAMIC_EXT = ".so"
	LINUX_BINARY_EXT  = ""

	OSX_STATIC_EXT  = ".a"
	OSX_DYNAMIC_EXT = ".dylib"
	OSX_BINARY_EXT  = ""
)

type ObakeBuildFolder struct {
	buildType      ObakeBuildType
	path           string
	name           string
	obakeBuildFile []byte
}

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
}

type StaticLibJSON struct {
	Name           string   `json:"name"`
	SrcFolders     []string `json:"srcFolders"`
	SrcExtension   string   `json:"srcExtension"`
	HeadersFolders []string `json:"headersFolders"`
	OutFolder      string   `json:"outFolder"`
}

type PluginJSON struct {
	Name           string   `json:"name"`
	SrcFolders     []string `json:"srcFolders"`
	SrcExtension   string   `json:"srcExtension"`
	HeadersFolders []string `json:"headersFolders"`
	OutFolder      string   `json:"outFolder"`
}

type BuilderJSON struct {
	Os            string   `json:"OS"`
	Binary        string   `json:"binary"`
	StaticLibs    []string `json:"staticLibs"`
	Plugins       []string `json:"plugins"`
	PluginsFolder string   `json:"pluginsFolder"`
	OutFolder     string   `json:"outFolder"`
}

type BinaryType struct {
	folderInfos ObakeBuildFolder
	name        string
	staticLibs  []string
	plugins     []string
	sources     []string
	headers     []string
}

type StaticLibType struct {
	folderInfos ObakeBuildFolder
	name        string
	staticLibs  []string
	plugins     []string
	sources     []string
	headers     []string
	outFolder   string
}

type PluginType struct {
	folderInfos ObakeBuildFolder
	name        string
	staticLibs  []string
	plugins     []string
	sources     []string
	headers     []string
	outFolder   string
}

type BuilderType struct {
	os            ObakeOSType
	binary        BinaryType
	staticLibs    []StaticLibType
	plugins       []PluginType
	pluginsFolder string
	outFolder     string
}

func getSrcAndHeadersFolderPath(folderInfos ObakeBuildFolder, srcFolder string, headersFolder string) (srcPath string, headersPath string) {
	srcPath = folderInfos.path + "/" + srcFolder
	headersPath = folderInfos.path + "/" + headersFolder
	return
}

func getAllFilesFromDir(folderPath string, extension string) (finalFiles []string) {
	files, err := ioutil.ReadDir(folderPath)

	if err != nil {
		log.Fatal(err)

		return
	}

	fmt.Printf("Hellow\n")

	for _, file := range files {
		filename := file.Name()

		if strings.Contains(filename, extension) {
			finalFiles = append(finalFiles, filename)
		}
	}

	return
}

func getBuildFileJSONObj(folder ObakeBuildFolder) ObjectJSON {
	var subFolderObakeJSON ObjectJSON
	json.Unmarshal(folder.obakeBuildFile, &subFolderObakeJSON)
	return subFolderObakeJSON
}

func getOsType(osStr string) ObakeOSType {
	switch osStr {
	case "Windows":
		return WINDOWS
	case "Linux":
		return LINUX
	case "OSX":
		return OSX
	}
	return UNKNOWN
}

func exists(path string) (bool, error) {
	_, err := os.Stat(path)
	if err == nil {
		return true, nil
	}
	if os.IsNotExist(err) {
		return false, nil
	}
	return true, err
}

func joinAtBegin(toJoin string, strs []string) (finalStrs []string) {
	for _, str := range strs {
		finalStrs = append(finalStrs, toJoin+str)
	}

	return
}

func handleBinary(osType ObakeOSType, folderInfos ObakeBuildFolder) BinaryType {
	var binary BinaryType
	//	jsonObj := getBuildFileJSONObj(folderInfos)

	binary.folderInfos = folderInfos

	return binary
}

func handleStatic(osType ObakeOSType, folderInfos ObakeBuildFolder) StaticLibType {
	var staticLib StaticLibType
	var objectFilesPath []string
	jsonObj := getBuildFileJSONObj(folderInfos)

	staticLib.folderInfos = folderInfos
	staticLib.name = jsonObj.StaticLib.Name
	staticLib.outFolder = staticLib.folderInfos.path + "/" + jsonObj.StaticLib.OutFolder

	success, _ := exists(staticLib.outFolder)
	if !success {
		os.MkdirAll(staticLib.outFolder, os.ModePerm)
	}

	var sourceFiles []string
	var sourceFilesPath []string
	for _, srcFolder := range jsonObj.StaticLib.SrcFolders {
		sourceFilesInFolder := getAllFilesFromDir(folderInfos.path+"/"+srcFolder, jsonObj.StaticLib.SrcExtension)
		sourceFiles = append(sourceFiles, sourceFilesInFolder...)
		fmt.Printf("SourceFiles: %v\n", sourceFiles)
		sourceFilesPath = append(sourceFilesPath, joinAtBegin(srcFolder+"/", sourceFilesInFolder)...)
		fmt.Printf("SourceFilesPath: %v\n", sourceFilesPath)
		fmt.Printf("SourceFiles: %v\n", sourceFiles)
	}
	fmt.Printf("StaticLib srcFolders: %v | headersFolder: %v\n", jsonObj.StaticLib.SrcFolders, jsonObj.StaticLib.HeadersFolders)

	for i, srcFilePath := range sourceFilesPath {
		oFilePath := staticLib.outFolder + "/" + strings.Replace(sourceFiles[i], jsonObj.StaticLib.SrcExtension, ".o", -1)
		objectFilesPath = append(objectFilesPath, oFilePath)

		args := []string{"-c", folderInfos.path + "/" + srcFilePath, "-o", oFilePath}

		for _, headerFolder := range jsonObj.StaticLib.HeadersFolders {
			args = append(args, "-I"+folderInfos.path+"/"+headerFolder+"/")
		}

		fmt.Printf("args: %v\n", args)
		exec.Command("clang++", args...).Run()
	}

	staticLibExtension := LINUX_STATIC_EXT
	if osType == WINDOWS {
		staticLibExtension = WINDOWS_STATIC_EXT
	}

	args := []string{"rcs", staticLib.outFolder + "/" + staticLib.name + staticLibExtension}

	args = append(args, objectFilesPath...)

	fmt.Printf("args: %v\n", args)

	out, err := exec.Command("ar", args...).Output()

	if err != nil {
		fmt.Printf("StaticLib: %s | Error: %s\n", staticLib.name, err)
	} else {
		fmt.Printf("Out: %s\n", out)
	}

	return staticLib
}

func handlePlugin(osType ObakeOSType, folderInfos ObakeBuildFolder) PluginType {
	var plugin PluginType
	//	jsonObj := getBuildFileJSONObj(folderInfos)

	plugin.folderInfos = folderInfos

	return plugin
}

func handleBuilder(BuilderJSON BuilderJSON, osType ObakeOSType, binary BinaryType,
	staticLibs []StaticLibType, plugins []PluginType) BuilderType {
	var Builder BuilderType

	if BuilderJSON.Binary == binary.name {
		for i, staticLib := range BuilderJSON.StaticLibs {
			if staticLib == staticLibs[i].name {
				Builder.staticLibs = append(Builder.staticLibs, staticLibs[i])
			}
		}

		for i, plugin := range BuilderJSON.Plugins {
			if plugin == plugins[i].name {
				Builder.plugins = append(Builder.plugins, plugins[i])
			}
		}

		Builder.pluginsFolder = BuilderJSON.PluginsFolder
		Builder.os = osType
		Builder.binary = binary
		Builder.outFolder = BuilderJSON.OutFolder
	}

	return Builder
}

func build(Builder BuilderType) {

}

func handleFiles(rootOBSFile []byte, subFiles []ObakeBuildFolder) {
	var binary BinaryType
	var staticLibs []StaticLibType
	var plugins []PluginType
	var Builder BuilderType
	var obakeRootFileObj ObjectJSON

	json.Unmarshal(rootOBSFile, &obakeRootFileObj)
	fmt.Printf("RootOBSFile: %v\n", obakeRootFileObj)

	if obakeRootFileObj.Builder.Binary != "" {
		var osType = getOsType(obakeRootFileObj.Builder.Os)

		fmt.Printf("OsType: %s\n", obakeRootFileObj.Builder.Os)
		fmt.Printf("SubFilesNB: %d\n", len(subFiles))

		if osType != UNKNOWN {
			for _, buildFolder := range subFiles {

				fmt.Printf("ReadFile: %s\n", "./"+buildFolder.name+"/"+obakeBSFilename)
				buildFolder.obakeBuildFile, _ = ioutil.ReadFile("./" + buildFolder.name + "/" + obakeBSFilename)
				obakeCurrentFile := getBuildFileJSONObj(buildFolder)

				if obakeCurrentFile.Binary.Name != "" {
					buildFolder.buildType = BINARY
					binary = handleBinary(osType, buildFolder)
				} else if obakeCurrentFile.Plugin.Name != "" {
					buildFolder.buildType = PLUGIN
					plugins = append(plugins, handlePlugin(osType, buildFolder))
				} else if obakeCurrentFile.StaticLib.Name != "" {
					buildFolder.buildType = STATIC_LIB
					staticLibs = append(staticLibs, handleStatic(osType, buildFolder))
				}
			}
			Builder = handleBuilder(obakeRootFileObj.Builder, osType, binary, staticLibs, plugins)
			build(Builder)
		}
	}

	/*			fmt.Printf("SubfilesNB: %d\n", len(subFiles))

				for i := 0; i < len(subFiles); i++ {
					json.Unmarshal(subFiles[i].obakeBuildFile, &jsontype)
					fmt.Printf("	Subfile[%d]: %v\n", i, jsontype)
				}*/

	//m := new(Dispatch)
	//var m interface{}
	/*	var jsontype jsonobject
		json.Unmarshal(file, &jsontype)
		fmt.Printf("Results: %v\n", jsontype)
	*/

	/*	files, err := ioutil.ReadDir(".")
		if err != nil {
			log.Fatal(err)
		}
	*/
	/*
		var fullArgs []string

		var sourceFiles []string

		for _, file := range files {
			filename := file.Name()

			if strings.Contains(filename, obakeFileObj.Binary.SrcExtension) {
				fmt.Printf("Append: %s\n", filename)
				sourceFiles = append(sourceFiles, filename)
			}
		}

		endArgs := []string{"-o", string(obakeFileObj.Binary.Name + ".exe")}

		fullArgs = append(fullArgs, sourceFiles...)
		fullArgs = append(fullArgs, endArgs...)

		out, err := exec.Command("clang++", fullArgs...).Output()

		fmt.Printf("FullArgs: %v\n", fullArgs)

		if err != nil {
			fmt.Printf("Error: %s\n", err)
		} else {
			fmt.Printf("Command: clang++ %s\n", strings.Join(fullArgs, " "))
			fmt.Printf("Out: %s\n", out)
		}*/
}

func main() {
	//	var argsWithProg []string = os.Args
	var subFiles []ObakeBuildFolder
	var rootOBSFile []byte
	var subOBSFile []byte

	// argsWithProg = os.Args[1:]

	files, err := ioutil.ReadDir(".")
	if err != nil {
		log.Fatal(err)
	}

	for _, file := range files {
		filename := file.Name()
		if file.IsDir() {
			filetest := "./" + filename
			fmt.Printf("DirFound: %s\n", filetest)
			files, err := ioutil.ReadDir(filetest)
			if err == nil {
				subfolderName := filename
				for _, file := range files {
					filename = file.Name()
					if filename == obakeBSFilename {

						fmt.Printf("	Obake SubBuild File Found\n")
						subOBSFile, _ = ioutil.ReadFile(filename)
						var subFolderInfo ObakeBuildFolder
						subFolderInfo.buildType = NONE
						subFolderInfo = ObakeBuildFolder{path: "./" + subfolderName, name: subfolderName, obakeBuildFile: subOBSFile}
						subFiles = append(subFiles, subFolderInfo)
					}
				}
			} else {
				fmt.Printf("ERR: %s\n", err)
				log.Fatal(err)
			}
		} else if filename == obakeBSFilename {
			rootOBSFile, _ = ioutil.ReadFile(filename)
			fmt.Printf("Obake RootBuild File Found\n\n")

		}
	}
	/*
		}*/

	handleFiles(rootOBSFile, subFiles)

	//	fmt.Println(e)
	/*
		file, e := ioutil.ReadFile("./config.json")
		if e != nil {
			fmt.Printf("File error: %v\n", e)
		}
		fmt.Printf("%s\n", string(file))

		//m := new(Dispatch)
		//var m interface{}
		var jsontype jsonobject
		json.Unmarshal(file, &jsontype)
		fmt.Printf("Results: %v\n", jsontype)
	*/
	/*	out, err := exec.Command("clang++", argsWithProg...).Output()

		if err != nil {
			fmt.Printf("Error: %s\n", err)
		} else {
			fmt.Printf("Out: %s\n", out)
		}*/
}
