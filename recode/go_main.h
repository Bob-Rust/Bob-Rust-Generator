#pragma once

/*
var (
	Input      string
	Outputs    flagArray
	Background string
	Configs    shapeConfigArray
	Alpha      int
	InputSize  int
	OutputSize int
	Mode       int
	Workers    int
	Nth        int
	Repeat     int
	V, VV      bool
)

type flagArray []string

func (i *flagArray) String() string {
	return strings.Join(*i, ", ")
}

func (i *flagArray) Set(value string) error {
	*i = append(*i, value)
	return nil
}

type shapeConfig struct {
	Count  int
	Mode   int
	Alpha  int
	Repeat int
}

type shapeConfigArray []shapeConfig

func (i *shapeConfigArray) String() string {
	return ""
}

func (i *shapeConfigArray) Set(value string) error {
	n, _ := strconv.ParseInt(value, 0, 0)
	*i = append(*i, shapeConfig{int(n), Mode, Alpha, Repeat})
	return nil
}

func init() {
	flag.StringVar(&Input, "i", "", "input image path")
	flag.Var(&Outputs, "o", "output image path")
	flag.Var(&Configs, "n", "number of primitives")
	flag.StringVar(&Background, "bg", "", "background color (hex)")
	flag.IntVar(&Alpha, "a", 128, "alpha value")
	flag.IntVar(&InputSize, "r", 256, "resize large input images to this size")
	flag.IntVar(&OutputSize, "s", 1024, "output image size")
	flag.IntVar(&Mode, "m", 1, "0=combo 1=triangle 2=rect 3=ellipse 4=circle 5=rotatedrect 6=beziers 7=rotatedellipse 8=polygon")
	flag.IntVar(&Workers, "j", 0, "number of parallel workers (default uses all cores)")
	flag.IntVar(&Nth, "nth", 1, "save every Nth frame (put \"%d\" in path)")
	flag.IntVar(&Repeat, "rep", 0, "add N extra shapes per iteration with reduced search")
	flag.BoolVar(&V, "v", false, "verbose")
	flag.BoolVar(&VV, "vv", false, "very verbose")
}

func errorMessage(message string) bool {
	fmt.Fprintln(os.Stderr, message)
	return false
}

func check(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
*/

constexpr int GO_MAIN_SUCCESSFULL = 1;
constexpr int GO_MAIN_ERROR = 0;

#include <chrono>
using std::chrono::high_resolution_clock;
using namespace std::chrono;

#include "log.h"
#include "util.h"
#include "bundle.h"
#include "model.h"

int go_main(int argc, char** argv) {
	char* input = argv[0]; // First argument is the input file

	v("Reading from the file '%s'\n", input);

	Image* image = LoadImage(input);
	if(!image) {
		v("Failed to read image '%s'\n", input);
		return GO_MAIN_ERROR;
	}

	int OutputSize = 1; // Get this from the commandline
	int numWorkers = 7; // Get this from the commandline
	Color bg{0,0,0,0xff};//Get this from the commandline 
	

	// Start the algorithm
	Model* model = new Model(image, bg, OutputSize, numWorkers);
	int Count = 4000;
	ShapeType Mode = ShapeType::ShapeTypeCircle;
	int Alpha = 72;
	int Repeat = 1;
	v("count=%d, mode=%d, alpha=%d, repeat=%d\n", Count, Mode, Alpha, Repeat);
	
	//auto start = chrono::high_resolution_clock::now();
	//auto end = chrono::high_resolution_clock::now();
	//auto dur = end - begin;
    //auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
	

	auto begin = high_resolution_clock::now();
	int frame = 0;
	for(int i = 0; i < Count; i++) {
		frame++;

		// Find optimal shape and add it to the model
		auto start = high_resolution_clock::now();
		
		// t := time.Now()
		int n = model->Step(Mode, Alpha, Repeat);
		auto end = high_resolution_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start); 
		float nps = n / (duration_cast<milliseconds>(end - begin).count() + 0.0f); 


		if((i % 40) == 0) {
			v("%d: t=%.3f, score=%.6f, n=%d, n/s=%.2f\n", frame, elapsed.count() / 1000.0, model->score, n, nps);

			std:stringstream stream;
			stream << "C:/Users/Admin/source/repos/BorstPrimitive/Debug/outfolder/image" << "_" << i << ".png";
		
			std::string str(stream.str());
			char* stream_path = (char*)str.c_str();

			v("Save model to: '%s'\n", stream_path);
			SavePNG(stream_path, &model->current[0]);
		}

		/*
		// write output image(s)
		for _, output := range Outputs {
			ext := strings.ToLower(filepath.Ext(output))
			if output == "-" {
				ext = ".svg"
			}
			percent := strings.Contains(output, "%")
			saveFrames := percent && ext != ".gif"
			// Changed to be forced just for seeing progress.
			saveFrames = frame%Nth == 0
			last := j == len(Configs)-1 && i == config.Count-1
			if saveFrames || last {
				path := output
				if percent {
					path = fmt.Sprintf(output, frame)
				}
				primitive.Log(1, "writing %s\n", path)
				switch ext {
				default:
					check(fmt.Errorf("unrecognized file extension: %s", ext))
				case ".png":
					check(primitive.SavePNG(path, model.Context.Image()))
				case ".svg":
					check(primitive.SaveFile(path, model.SVG()))
				case ".borst":
					check(primitive.SavePNG(path+".png", model.Context.Image()))
					//check(primitive.SaveFile(path + ".svg", model.SVG()))
					check(primitive.SaveFile(path, model.BOSRT()))
				}
			}
		}
		*/
	}

	return GO_MAIN_SUCCESSFULL;
}