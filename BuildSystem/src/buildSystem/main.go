package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"os/exec"
	"strings"
)

func buildAndGetObjectFiles(sourceFilesPath []string, sourceFiles []string, headersFolders []string,
	externIncludes []string, extension string, outFolder string, folderInfos ObakeBuildFolder,
	staticLibs []string, allLibs []StaticLibType) (objectFilesPath []string) {
	for i, srcFilePath := range sourceFilesPath {
		oFilePath := outFolder + "/" + strings.Replace(sourceFiles[i], extension, ".o", -1)
		objectFilesPath = append(objectFilesPath, oFilePath)

		args := []string{"-c", folderInfos.path + "/" + srcFilePath, "-o", oFilePath}
		args = append(args, compilerFlags...)

		for _, headerFolder := range headersFolders {
			if headerFolder == "." {
				args = append(args, "-I"+folderInfos.path+"/")
			} else {
				args = append(args, "-I"+folderInfos.path+"/"+headerFolder+"/")
			}
		}

		//		linkPaths, linkNames, linkIncludes := getStaticLibsLinks(staticLibs, allLibs, folderInfos.name)
		_, _, linkIncludes := getStaticLibsLinks(staticLibs, allLibs, folderInfos.name)

		args = append(args, linkIncludes...)

		if externIncludes != nil {
			for _, externInclude := range externIncludes {
				args = append(args, "-I"+"./"+externInclude+"/")
			}
		}

		fmt.Printf("Obj files: %s %v\n", toolchain, args)
		exec.Command(toolchain, args...).Run()
	}

	return
}

func handleBinary(binary BinaryType, allLibs []StaticLibType) bool {

	return true
}

func handleStatic(staticLib StaticLibType, allLibs []StaticLibType) bool {

	objectFilesPath := buildAndGetObjectFiles(staticLib.sources, staticLib.sourceFileNames, staticLib.headerFolders,
		staticLib.externIncludes, staticLib.sourceExtension, staticLib.outFolder,
		staticLib.folderInfos, staticLib.staticLibs, allLibs)

	staticLibExtension := getStaticLibOSExtension()

	args := []string{"rcs", staticLib.outFolder + "/" + staticLib.name + staticLibExtension}

	args = append(args, objectFilesPath...)

	linkPaths, linkNames, linkIncludes := getStaticLibsLinks(staticLib.staticLibs, allLibs, staticLib.name)

	args = append(args, linkIncludes...)
	args = append(args, linkPaths...)
	args = append(args, linkNames...)

	fmt.Printf("Handle Static args: %v\n", args)

	out, err := exec.Command("ar", args...).Output()

	if err != nil {
		fmt.Printf("StaticLib: %s | Error: %s\n", staticLib.name, err)
		return false
	} else {
		fmt.Printf("Out: %s\n", out)
	}

	staticLib.isBuilt = true
	return true
}

func handlePlugin(plugin PluginType, allLibs []StaticLibType) bool {

	objectFilesPath := buildAndGetObjectFiles(plugin.sources, plugin.sourceFileNames, plugin.headerFolders,
		plugin.externIncludes, plugin.sourceExtension, plugin.outFolder, plugin.folderInfos,
		plugin.staticLibs, allLibs)

	pluginLibExtension := LINUX_DYNAMIC_EXT
	if osType == WINDOWS {
		pluginLibExtension = WINDOWS_DYNAMIC_EXT
	} else if osType == OSX {
		pluginLibExtension = OSX_DYNAMIC_EXT
	}

	args := []string{"-shared", "-o", plugin.outFolder + "/" + plugin.name + pluginLibExtension}

	args = append(args, compilerFlags...)
	args = append(args, objectFilesPath...)

	linkPaths, linkNames, linkIncludes := getStaticLibsLinks(plugin.staticLibs, allLibs, "")

	args = append(args, linkIncludes...)
	args = append(args, linkPaths...)
	args = append(args, linkNames...)

	fmt.Printf("Handle plugin args: %v\n", args)

	out, err := exec.Command(toolchain, args...).Output()

	if err != nil {
		fmt.Printf("PluginLib: %s | Error: %s\n", plugin.name, err)
		return false
	} else {
		fmt.Printf("Out: %s\n", out)
	}

	return true
}

func handleBuilder(BuilderJSON BuilderJSON, binaries []BinaryType,
	staticLibs []StaticLibType, plugins []PluginType) BuilderType {
	var Builder BuilderType

	return Builder
}

func build(Builder BuilderType) {

}

func handleFiles(rootOBSFile []byte, subFiles []ObakeBuildFolder) {
	var binaries []BinaryType
	var staticLibs []StaticLibType
	var plugins []PluginType
	var Builder BuilderType
	var obakeRootFileObj ObjectJSON

	json.Unmarshal(rootOBSFile, &obakeRootFileObj)
	fmt.Printf("RootOBSFile: %v\n", obakeRootFileObj)

	if obakeRootFileObj.Builder.Binary != "" {
		osType = getOsType(obakeRootFileObj.Builder.Os)
		compilerFlags = obakeRootFileObj.Builder.CompilerFlags

		if isValidToolchain(obakeRootFileObj.Builder.Toolchain) {
			toolchain = obakeRootFileObj.Builder.Toolchain
		} else {
			toolchain = DEFAULT_TOOLCHAIN
		}
		fmt.Printf("OsType: %s, | Toolchain:%s\n", obakeRootFileObj.Builder.Os, toolchain)
		fmt.Printf("SubFilesNB: %d\n", len(subFiles))

		if osType != UNKNOWN {
			for _, buildFolder := range subFiles {

				fmt.Printf("ReadFile: %s\n", "./"+buildFolder.name+"/"+OBAKE_BS_FILENAME)
				buildFolder.obakeBuildFile, _ = ioutil.ReadFile("./" + buildFolder.name + "/" + OBAKE_BS_FILENAME)
				obakeCurrentFile := getBuildFileJSONObj(buildFolder)

				if obakeCurrentFile.Binary.Name != "" {
					buildFolder.buildType = BINARY
					binaries = append(binaries, makeBinaryType(buildFolder))
				} else if obakeCurrentFile.Plugin.Name != "" {
					buildFolder.buildType = PLUGIN
					plugins = append(plugins, makePluginType(buildFolder))
				} else if obakeCurrentFile.StaticLib.Name != "" {
					buildFolder.buildType = STATIC_LIB
					staticLibs = append(staticLibs, makeStaticLibType(buildFolder))
				}
			}

			for _, staticType := range staticLibs {
				if handleStatic(staticType, staticLibs) == false {
				}
			}
			for _, pluginType := range plugins {
				if handlePlugin(pluginType, staticLibs) == false {
				}
			}
			for _, binaryType := range binaries {
				if handleBinary(binaryType, staticLibs) == false {
				}
			}

			Builder = handleBuilder(obakeRootFileObj.Builder, binaries, staticLibs, plugins)
			build(Builder)
		}
	}
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
					if filename == OBAKE_BS_FILENAME {

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
		} else if filename == OBAKE_BS_FILENAME {
			rootOBSFile, _ = ioutil.ReadFile(filename)
			fmt.Printf("Obake RootBuild File Found\n\n")

		}
	}

	handleFiles(rootOBSFile, subFiles)
}
