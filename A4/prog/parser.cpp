
#define T_TRI		1
#define T_LIG		2
#define T_SPHERE	3
#define T_PLANE		4

#define F_PUSH(X)	f_tmp = X; data.push_back(f_tmp);
#define D_PUSH		file >> f_tmp; data.push_back(f_tmp);
#define GETDATA		D_PUSH D_PUSH D_PUSH D_PUSH D_PUSH D_PUSH D_PUSH D_PUSH D_PUSH 
#define GOCLOSE		while (true) {getline(file,line);if(!line.find('}')){break;} cout << "EXTRA: " << line << endl; }

//Simply parses input.
using namespace std;

ifstream file=ifstream(filename);
string line;
vector<GLfloat> data;
GLfloat f_tmp;
void parse(string filename){
	file=ifstream(filename);
	while (file){
		getline(file,line); //Reads a line from file and puts it in line.
		if(line[0] == '#'){continue;}
		else if(line.size() <= 2){continue;}
		else if(!line.find("triangle")){
			cout << "Parsing triangle" << endl;
			F_PUSH(T_TRI);
			D_PUSH;
			GOCLOSE;
		}
		else if(!line.find("sphere")){
			F_PUSH(T_SPHERE);
			D_PUSH;
			GOCLOSE;
		}
		
		else{cout << "Unparsed: " << line << endl;}
	}
	file.close();
}


