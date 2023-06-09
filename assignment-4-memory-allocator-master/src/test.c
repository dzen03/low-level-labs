#define _DEFAULT_SOURCE

#include "test.h"

#define HEAP_SIZE 1000

#define ALLOC_SIZE_DEFAULT 100
#define BUFFER_LENGTH 2048

typedef struct
{
  bool result;
  char message[256];
} test_result;

static struct block_header* get_block(void* block) {
  return (struct block_header*) (block - offsetof(struct block_header, contents));
}


test_result test_normal(FILE* f)
{
  void* heap = heap_init(HEAP_SIZE);

  debug_heap(f, heap);
  if (heap == NULL)
    return (test_result){false, "Can't init heap"};

  void* mem = _malloc(ALLOC_SIZE_DEFAULT);
  debug_heap(f, heap);
  struct block_header *block = get_block(mem);

  if (mem == NULL || block->capacity.bytes != ALLOC_SIZE_DEFAULT)
    return (test_result){false, "Can't allocate memory"};

  _free(mem);
  debug_heap(f, heap);

  return (test_result){true};
}

test_result test_free_one(FILE* f)
{
  void* heap = heap_init(HEAP_SIZE);

  debug_heap(f, heap);
  if (heap == NULL)
    return (test_result){false, "Can't init heap"};

  void* mem1 = _malloc(ALLOC_SIZE_DEFAULT);
  debug_heap(f, heap);
  struct block_header *block = get_block(mem1);

  if (mem1 == NULL || block->capacity.bytes != ALLOC_SIZE_DEFAULT)
    return (test_result){false, "Can't allocate memory"};

  _free(mem1);
  debug_heap(f, heap);

  if (block->is_free != true)
    return (test_result){false, "Can't free block"};

  void* mem2 = _malloc(ALLOC_SIZE_DEFAULT);
  debug_heap(f, heap);
  struct block_header *block2 = get_block(mem2);

  if (mem2 == NULL || block2->capacity.bytes != ALLOC_SIZE_DEFAULT)
    return (test_result){false, "Can't allocate memory"};

  _free(mem2);
  debug_heap(f, heap);

  return (test_result){true};
}

test_result test_free_two(FILE* f)
{
  void* heap = heap_init(HEAP_SIZE);

  debug_heap(f, heap);
  if (heap == NULL)
    return (test_result){false, "Can't init heap"};

  void* mem1 = _malloc(ALLOC_SIZE_DEFAULT);
  debug_heap(f, heap);
  struct block_header *block = get_block(mem1);

  if (mem1 == NULL || block->capacity.bytes != ALLOC_SIZE_DEFAULT)
    return (test_result){false, "Can't allocate memory"};



  void* mem2 = _malloc(ALLOC_SIZE_DEFAULT);
  debug_heap(f, heap);
  struct block_header *block2 = get_block(mem2);

  if (mem2 == NULL || block2->capacity.bytes != ALLOC_SIZE_DEFAULT)
    return (test_result){false, "Can't allocate memory"};

  _free(mem2);
  debug_heap(f, heap);

  if (block2->is_free != true)
    return (test_result){false, "Can't free block"};


  _free(mem1);
  debug_heap(f, heap);

  if (block->is_free != true)
    return (test_result){false, "Can't free block"};

  void* mem3 = _malloc(ALLOC_SIZE_DEFAULT);
  debug_heap(f, heap);
  struct block_header *block3 = heap;

  if (mem3 == NULL || block3->capacity.bytes != ALLOC_SIZE_DEFAULT)
    return (test_result){false, "Can't allocate memory"};

  _free(mem3);
  debug_heap(f, heap);

  return (test_result){true};
}

test_result test_expand(FILE* f) {
  void* heap = heap_init(HEAP_SIZE);

  debug_heap(f, heap);
  if (heap == NULL)
    return (test_result){false, "Can't init heap"};

  void* mem1 = _malloc(HEAP_SIZE + 1);
  debug_heap(f, heap);
  struct block_header *block = get_block(mem1);

  if (mem1 == NULL || block->capacity.bytes != HEAP_SIZE + 1)
    return (test_result){false, "Can't allocate memory over limit"};

  void* mem2 = _malloc(ALLOC_SIZE_DEFAULT);
  debug_heap(f, heap);
  struct block_header *block2 = get_block(mem2);

  if (mem2 == NULL || block2->capacity.bytes != ALLOC_SIZE_DEFAULT)
    return (test_result){false, "Can't allocate memory"};

  _free(mem1);
  _free(mem2);
  debug_heap(f, heap);

  return (test_result){true};
}

test_result test_expand_diffent_region(FILE* f) {
  void* heap = heap_init(HEAP_SIZE);

  debug_heap(f, heap);
  if (heap == NULL)
    return (test_result){false, "Can't init heap"};

  void* mem1 = _malloc(HEAP_SIZE);
  debug_heap(f, heap);
  struct block_header *block = get_block(mem1);

  if (mem1 == NULL || block->capacity.bytes != HEAP_SIZE)
    return (test_result){false, "Can't allocate memory"};

  (void) mmap( (void*) heap + HEAP_SIZE, HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED , -1, 0 );

  void* mem2 = _malloc(ALLOC_SIZE_DEFAULT);
  debug_heap(f, heap);
  struct block_header *block2 = get_block(mem2);

  if (mem2 == NULL || block2->capacity.bytes != ALLOC_SIZE_DEFAULT)
    return (test_result){false, "Can't allocate memory"};

  _free(mem1);
  _free(mem2);
  debug_heap(f, heap);

  return (test_result){true};
}



char* itoa(int value, char* result, int base) {
  // check that the base if valid
  if (base < 2 || base > 36) { *result = '\0'; return result; }

  char* ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
  } while ( value );

  // Apply negative sign
  if (tmp_value < 0) *ptr++ = '-';
  *ptr-- = '\0';
  while(ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr--= *ptr1;
    *ptr1++ = tmp_char;
  }
  return result;
}

#define GET_TEST_NAME(test) (#test)


typedef test_result (*test)(FILE*);
test tests[20] = {test_normal, test_free_one, test_free_two, test_expand, test_expand_diffent_region}; // Array of tests to be run

bool run_all_tests(FILE* file)
{
  bool res = true;
  for (size_t i = 0; *(tests + i) != NULL; ++i)
  {
    char* name = malloc(sizeof(char) * 128);
    name = itoa((int)i, name, 10);

    FILE* f = fopen(name, "wb");

    test_result result = (*(tests + i))(f);
    res &= result.result;
    if (result.result == false)
    {
      fprintf(file, "Test #%zu failed with message:\n%s\nAdditional data stored in %s", i + 1, result.message, name);
    }
    fclose(f);
    free(name);
  }
  return res;
}

