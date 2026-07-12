#ifndef MAIN_H
#define MAIN_H
/**
 * struct dog - Dog structure
 * @name: Dog name
 * @age: Dog age
 * @owner: Dog owner
 */
struct dog
{
	char *name;
	float age;
	char *owner;
};
void init_dog(struct dog *d, char *name, float age, char *owner);
void print_dog(struct dog *d);
typedef struct dog dog_t;

#endif
