# 1 "/home/nox/Code/altitude/trunk/vm/programs/mem.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/home/nox/Code/altitude/trunk/vm/programs/mem.c"
void* __altitude_malloc(int);


void __altitude_putint(int);


int main(){
  int *array;
  void* memory;
  __altitude_malloc(10);
  memory = __altitude_malloc(20);
  array = (int*)memory;
  array[0] = 15;
  return array[0];
}
