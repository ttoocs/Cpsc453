//Scott Saunders, 10163541
//Provides OpenGL helpers

// #include <GL/glew.h>
// #include <GLFW/glfw3.h>

using namespace std;


#define lcase(X)	case(X): cout << #X << endl; break;


// reports GLFW errors
void ErrorCallback(int error, const char* description)
 {
     cout << "GLFW ERROR " << error << ":" << endl;
     cout << description << endl;
 }


void check_glerror(){
	GLenum error = (glGetError());
	switch (error){
		case(GL_NO_ERROR) :
		default :
//			cout << GL_NO_ERROR << ":" << error << endl;
			return;
		case(GL_INVALID_ENUM):
			cout << "GL_INVALID_ENUM" << endl;
			break;
		case(GL_INVALID_VALUE):
			cout << "GL_INVALID_VALUE" << endl;
			break;
		case(GL_INVALID_OPERATION):
			cout << "GL_INVALID_OPERATION" << endl;
			break;
		case(GL_INVALID_FRAMEBUFFER_OPERATION):
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl;
			break;
		case(GL_OUT_OF_MEMORY):
			cout << "GL_OUT_OF_MEMORY" << endl;
			break;
		case(GL_STACK_UNDERFLOW):
			cout << "GL_STACK_UNDERFLOW" << endl;
			break;
		case(GL_STACK_OVERFLOW):
			cout << "GL_STACK_OVERFLOW" <<endl;
			break;
	}
}

void check_compile(GLuint vertexShader){
	    GLint isCompiled = 0;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
        if(isCompiled == GL_FALSE)
        {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
		std::string infoLog(maxLength, ' ');
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
        cout << infoLog << endl;
        return;
        }
	
}

GLFWwindow * glfw_init(int X, int Y, char const * Title){  //Initalizes and returns a GLFW window.
    glfwSetErrorCallback(ErrorCallback);
	
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	GLFWwindow * window = glfwCreateWindow(X,Y, Title, nullptr, nullptr);

	glfwMakeContextCurrent(window); //Sets up a OpenGL context

	return( window); // Windowed
//	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
	
}


// TAKEN FROM BOILERPLATE
// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
    string source;

    ifstream input(filename.c_str());
    if (input) {
        copy(istreambuf_iterator<char>(input),
            istreambuf_iterator<char>(),
            back_inserter(source));
        input.close();
    }
    else {
        cout << "ERROR: Could not load shader source from file "
            << filename << endl;
    }
//	cout << "RAW READ: " << source << " : END READ" << endl;
    return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
    // allocate shader object name
    GLuint shaderObject = glCreateShader(shaderType);

    // try compiling the source as a shader of the given type
    const GLchar *source_ptr = source.c_str();
    glShaderSource(shaderObject, 1, &source_ptr, 0);
    glCompileShader(shaderObject);

    // retrieve compile status
    GLint status;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
        cout << "ERROR compiling shader:" << endl << endl;
        cout << source << endl;
        cout << info << endl;
    }

    return shaderObject;
}

//  linked from vertex and fragment shaders
GLuint LinkProgram(GLuint programObject)
{


    // try linking the program with given attachments
    glLinkProgram(programObject);

    // retrieve link status
    GLint status;
    glGetProgramiv(programObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
        cout << "ERROR linking shader program:" << endl;
        cout << info << endl;
    }

    return programObject;
}

void check_gllink(GLuint programObject){
    GLint status;
    glGetProgramiv(programObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
        cout << "ERROR linking program:" << endl;
        cout << info << endl;
		exit(32);
    }
}








// https://www.opengl.org/wiki/Debug_Output#Getting_messages //

void GL_error_callback(GLenum source, GLenum type, GLuint id,
   GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
	if( true && severity != GL_DEBUG_SEVERITY_NOTIFICATION){

	cout << "GL ERROR CALLBACK: " << endl;
	cout << "Source: " << source << " : ";
	switch (source){
		lcase(GL_DEBUG_SOURCE_API)
		lcase(GL_DEBUG_SOURCE_WINDOW_SYSTEM)
		lcase(GL_DEBUG_SOURCE_SHADER_COMPILER)
		lcase(GL_DEBUG_SOURCE_THIRD_PARTY)
		lcase(GL_DEBUG_SOURCE_APPLICATION)
		lcase(GL_DEBUG_SOURCE_OTHER)				//THESE ARN'T DEFINED?
	}  

	cout << "Type: " <<  type << " : ";
	switch (type){
		lcase(GL_DEBUG_TYPE_ERROR)
		lcase(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
		lcase(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
		lcase(GL_DEBUG_TYPE_PORTABILITY)
		lcase(GL_DEBUG_TYPE_PERFORMANCE)
		lcase(GL_DEBUG_TYPE_MARKER)
		lcase(GL_DEBUG_TYPE_PUSH_GROUP)
		lcase(GL_DEBUG_TYPE_POP_GROUP)
		lcase(GL_DEBUG_TYPE_OTHER)
	}
		
	cout << "ID: " << id << endl;
	cout << "Serverity: " << severity << " : ";
	switch (severity){
		lcase(GL_DEBUG_SEVERITY_HIGH)
		lcase(GL_DEBUG_SEVERITY_MEDIUM)
		lcase(GL_DEBUG_SEVERITY_LOW)
		lcase(GL_DEBUG_SEVERITY_NOTIFICATION)
	}
	cout << "Length: " << length << endl;
	cout << "Message: " << message << endl;
	cout << "UserParam: " << userParam << endl;
	cout << "END: GL ERROR CALLBACK: " << endl;

	}
}
