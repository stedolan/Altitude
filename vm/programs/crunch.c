int main(){
  int ret = 0;
  
  for (int i=0;i<1000;i++){
    for (int j=0;j<i;j++){
      ret += 30;
    }
    for (int j=1;j<i;j++){
      ret -= 30;
    }
  }
  return ret;
}
