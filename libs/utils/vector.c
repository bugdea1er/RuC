/*
 *	Copyright 2021 Andrey Terekhov, Victor Y. Fadeev, Dmitrii Davladov
 *
 *	Licensed under the Apache License, Version 2.0 (the "License");
 *	you may not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 *	Unless required by applicable law or agreed to in writing, software
 *	distributed under the License is distributed on an "AS IS" BASIS,
 *	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	See the License for the specific language governing permissions and
 *	limitations under the License.
 */

#include "vector.h"
#include <stdlib.h>
#include <string.h>


vector vector_create(const size_t alloc)
{
	vector vec;

	vec.size = 0;
	vec.size_alloc = alloc != 0 ? alloc : 1;
	vec.array = malloc(vec.size_alloc * sizeof(item_t));

	return vec;
}


int vector_increase(vector *const vec, const size_t size)
{
	if (!vector_is_correct(vec))
	{
		return -1;
	}

	if (vec->size + size > vec->size_alloc)
	{
		const size_t alloc_new = vec->size + size > 2 * vec->size_alloc ? vec->size + size : 2 * vec->size_alloc;
		item_t *array_new = realloc(vec->array, alloc_new * sizeof(item_t));
		if (array_new == NULL)
		{
			return -1;
		}

		vec->size_alloc = alloc_new;
		vec->array = array_new;
	}

	memset(&vec->array[vec->size], 0, size * sizeof(item_t));
	vec->size += size;
	return 0;
}

size_t vector_add(vector *const vec, const item_t value)
{
	if (!vector_is_correct(vec))
	{
		return SIZE_MAX;
	}

	if (vec->size == vec->size_alloc)
	{
		item_t *array_new = realloc(vec->array, 2 * vec->size_alloc * sizeof(item_t));
		if (array_new == NULL)
		{
			return SIZE_MAX;
		}

		vec->size_alloc *= 2;
		vec->array = array_new;
	}

	vec->array[vec->size] = value;
	return vec->size++;
}

int vector_set(vector *const vec, const size_t index, const item_t value)
{
	if (!vector_is_correct(vec) || index >= vec->size)
	{
		return -1;
	}

	vec->array[index] = value;
	return 0;
}

item_t vector_get(const vector *const vec, const size_t index)
{
	if (!vector_is_correct(vec) || index >= vec->size)
	{
		return ITEM_MAX;
	}

	return vec->array[index];
}

item_t vector_remove(vector *const vec)
{
	if (!vector_is_correct(vec) || vec->size == 0)
	{
		return ITEM_MAX;
	}

	return vec->array[--vec->size];
}


size_t vector_size(const vector *const vec)
{
	return vector_is_correct(vec) ? vec->size : SIZE_MAX;
}

int vector_is_correct(const vector *const vec)
{
	return vec != NULL && vec->array != NULL;
}


int vector_clear(vector *const vec)
{
	if (!vector_is_correct(vec))
	{
		return -1;
	}

	free(vec->array);
	vec->array = NULL;

	return 0;
}
