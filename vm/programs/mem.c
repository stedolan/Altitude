void* __altitude_malloc(int sz);


int main(){
  void *p1, *p2, *p3;
  __altitude_malloc(2);
  p2 = __altitude_malloc(5);
  p3 = __altitude_malloc(-1);
  return 0;
}
