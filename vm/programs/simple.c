struct astruct{
  int thisval;
};

struct somestruct{
  int val;
  struct astruct obj;
};

int sum(int x){
  struct somestruct s;
  s.obj.thisval = 0;
  for (int i=0;i<x;i++){
    s.obj.thisval += i;
  }
  return s.obj.thisval;
}

int main(){
  return sum(40);
}
