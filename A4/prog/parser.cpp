
// #define debug

#define T_TRI		1
#define T_LIGHT		2
#define T_SPHERE	3
#define T_PLANE		4
#define T_POINT		5
#define T_PARTICLE	6

#define F_PUSH(X)	f_tmp = X; data.push_back(f_tmp);
#define D_PUSH		token >> f_tmp; data.push_back(f_tmp);
#define GETDATA		D_PUSH D_PUSH D_PUSH D_PUSH D_PUSH D_PUSH D_PUSH D_PUSH D_PUSH 

#define OBJSIZE		13

//Simply parses input into uniform object.
using namespace std;


std::vector<GLfloat> parse(std::string filename){
	std::vector<GLfloat> data;
	GLfloat f_tmp;
	std::string line;
//	string token;
	std::string token;
	int cnt;
	
	ifstream file=ifstream(filename);
	F_PUSH(0);
	while (file){
		getline(file,line); //Reads a line from file and puts it in line.
		if(line[0] == '#'){continue;}
		else if(line.size() <= 2){continue;}
		else if(!line.find("triangle")){
			#ifdef debug
			cout << "Parsing triangle" << endl;
			#endif
			F_PUSH(T_TRI);
		}
		else if(!line.find("sphere")){
			#ifdef debug
			cout << "Parsing sphere" << endl;
			#endif
			F_PUSH(T_SPHERE);
		}else if (!line.find("light")){
			#ifdef debug
			cout << "Parsing light" << endl;
			#endif
			F_PUSH(T_LIGHT);
		}else if (!line.find("plane")){
			#ifdef debug
			cout << "Parsing plane" << endl;
			#endif
			F_PUSH(T_PLANE);
		}else if (!line.find("particle")){
			#ifdef debug
            cout << "Parsing particle" << endl;
            #endif
            F_PUSH(T_PARTICLE);
		}else{cout << "Unparsed: " << line << endl; continue;}
		//Now set data-values.
		cnt=1;
		while (true) {
			getline(file,line); //Get next line.	
			if(!line.find('}')){break;} //Break if close bracket.
			#ifdef debug
			cout << "\tParsing line: " << line << endl;
			#endif
		 	std::istringstream iss (line);
			while(std::getline(iss,token,' ')){
				#ifdef debug
				cout << "\t\tParsing token: " << token << endl;
				#endif
				if(token[0] == ' '){continue;}
				if(token == ""){continue;}
				F_PUSH(stof(token));
				cnt++;
			}
		}
		while(cnt < OBJSIZE){
			#ifdef debug
			cout << "\t\tPushing blank to make uniform object size" << endl;
			#endif

			F_PUSH(0);	//Fill any extra space so all objects are same-size.
			cnt++;
		}

	}
	file.close();
	#ifdef debug
	int i =0;	//Print out data
	while(i < data.size()){
		cout << data.data()[i] << " ";
		i++;
		if((i-1)%OBJSIZE==0) cout << endl;
	}
	cout << endl;
	#endif
	data.data()[0] = (data.size()/OBJSIZE); //Make first index the number of objects.
	return(data);
}


