using namespace std;

int DIM_H = 1080;
int DIM_W = 1920;
const int max_str = 100;
const char application_name[100] = "Ma super application";

void set_config() {

  cout << "Welcome in my application, thank to set the weight of the window :"
       << endl
       << "Width : ";
  cin >> DIM_W;
  cout << "Height : ";
  cin >> DIM_H;
}