void* __altitude_malloc(int);
#define malloc __altitude_malloc

void __altitude_putint(int);
#define putint __altitude_putint

int main(){
  int *array;
  void* memory;
  malloc(10);
  memory = malloc(20);
  array = (int*)memory;
  array[0] = 15;
  return array[0];
}
