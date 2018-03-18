/*
# File_Name: test_dll.c
# Author:  Prithvi Teja Veeravalli <prithvi.veeravalli.colorado.edu>
# Description: This program contains 8 unit tests for all 8 functions. 
*/
#include <math.h>
#include <stdlib.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include "double_l_list.h"

#define TEST_SUCCESS 1

void test_insert_at_beginning(void **state)
{
        struct node head_list, *head_returned;
	uint32_t data = 80;
	uint32_t inserted_data;
	int status;

/*Preparing linked list*/

        head_list.next = NULL;
        head_list.prev = NULL;


/*testig starts from here*/
	head_returned = insert_at_beginning(&head_list, data);

	inserted_data = get_data(head_returned, 1);

	if(head_returned->next == NULL)
	{
		status = 0;
	}
	else if(inserted_data == data)
	{
		status = 1;
	}
	else
	{
		status = 0;
	}
	
	assert_int_equal(TEST_SUCCESS, status);
}

void test_insert_at_end(void **state)
{
        struct node head_list, *head_returned;
        uint32_t data = 90;
        uint32_t inserted_data;
        int status;

/*Preparing linked list*/

        head_list.next = NULL;
        head_list.prev = NULL;

/*testing starts from here*/
        insert_at_beginning(&head_list, 80);
	head_returned = insert_at_end(&head_list, data);

        inserted_data = get_data(head_returned, 2);

        if(head_returned->next == NULL)
        {
                status = 0;
        }
        else if(inserted_data == data)
        {
                status = 1;
        }
        else
        {
                status = 0;
        }

        assert_int_equal(TEST_SUCCESS, status);
}

void test_insert_at_position(void **state)
{
        struct node head_list, *head_returned;
        uint32_t data = 110;
        uint32_t inserted_data;
        int status;

/*Preparing linked list*/

        head_list.next = NULL;
        head_list.prev = NULL;

/*testing starts from here*/
        insert_at_beginning(&head_list, 80);
        insert_at_end(&head_list, 90);
	insert_at_end(&head_list, 110);
        head_returned = insert_at_position(&head_list, 3, data);

        inserted_data = get_data(head_returned, 3);

        if(head_returned->next == NULL)
        {
                status = 0;
        }
        else if(inserted_data == data)
        {
                status = 1;
        }
        else
        {
                status = 0;
        }

        assert_int_equal(TEST_SUCCESS, status);
}

void test_remove_at_beginning(void **state)
{
        struct node head_list, *head_returned;
        uint32_t data = 110;
        //uint32_t inserted_data;
        int status;

/*Preparing linked list*/

        head_list.next = NULL;
        head_list.prev = NULL;

/*testing starts from here*/
        insert_at_beginning(&head_list, data);
        head_returned = delete_at_beginning(&head_list);


        if(head_returned->next == NULL)
        {
                status = 1;
        }
        else
        {
                status = 0;
        }

        assert_int_equal(TEST_SUCCESS, status);
}

  void test_remove_at_end(void **state)
  {
          struct node head_list, *head_returned;
          uint32_t data = 110;
          uint32_t inserted_data1, inserted_data2;
          int status;
  
  /*Preparing linked list*/
  
          head_list.next = NULL;
          head_list.prev = NULL;
  
  /*testing starts from here*/
          insert_at_beginning(&head_list, data);
	  insert_at_end(&head_list, data + 10);
	  insert_at_end(&head_list, data + 90);

          head_returned = delete_at_end(&head_list);
	  inserted_data1 = get_data(head_returned, 1);
	  inserted_data2 = get_data(head_returned, 2);

	  if((inserted_data1 == data)&&(inserted_data2 == data + 10))
	  {
          	status = 1;
	  }
	  else
	  {
	  	status = 0;
          }
  
          assert_int_equal(TEST_SUCCESS, status);
  }


    void test_remove_at_position(void **state)
    {
            struct node head_list, *head_returned;
            uint32_t data = 110;
            uint32_t inserted_data_pre_remove, inserted_data_post_remove;
	    size_t position = 2;
            int status;
  
    /*Preparing linked list*/
  
            head_list.next = NULL;
            head_list.prev = NULL;
  
    /*testing starts from here*/
            insert_at_beginning(&head_list, data);
            insert_at_end(&head_list, data + 10);
            insert_at_end(&head_list, data + 90);

	    inserted_data_pre_remove = get_data(&head_list, position + 1); 	    
  
            head_returned = delete_at_position(&head_list, position);

            inserted_data_post_remove = get_data(head_returned, position);
  
            if((inserted_data_pre_remove == inserted_data_post_remove))
            {
                  status = 1;
            }
            else
            {
                  status = 0;
            }
  
            assert_int_equal(TEST_SUCCESS, status);
    }



  void test_size(void **state)
  {
          struct node head_list;
          uint32_t data = 110;
          size_t size1, size2, size3, size4;
          int status;

  /*Preparing linked list*/

          head_list.next = NULL;
          head_list.prev = NULL;

  /*testing starts from here*/

	  size1 = size(&head_list);
          insert_at_beginning(&head_list, data);
	  size2 = size(&head_list);	
          insert_at_end(&head_list, data + 10);
	  size3 = size(&head_list);
          insert_at_end(&head_list, data + 90);
          size4 = size(&head_list);

          if((size1 == 0)&&(size2 == 1)&&(size3 == 2)&&(size4 == 3))
          {
                status = 1;
          }
          else
          {
                status = 0;
          }

          assert_int_equal(TEST_SUCCESS, status);
  }



    void test_destroy(void **state)
    {
            struct node head_list, head_pre_destroy, *head_returned;
            uint32_t data = 110;
            
            int status;
  
    /*Preparing linked list*/
  
            head_list.next = NULL;
            head_list.prev = NULL;
  
    /*testing starts from here*/
  
            insert_at_beginning(&head_list, data);
            insert_at_end(&head_list, data + 10);
            insert_at_end(&head_list, data + 90);

	    memcpy(&head_pre_destroy, &head_list, sizeof(struct node));

            head_returned = destroy(&head_list);

  
            if((head_returned->next == NULL)&&(head_pre_destroy.next))
            {
                  status = 1;
            }
            else
            {
                  status = 0;
            }
  
            assert_int_equal(TEST_SUCCESS, status);
    }
int main(int argc, char **argv)
{



  const struct CMUnitTest unit_tests[] = {
    cmocka_unit_test(test_insert_at_beginning),
    cmocka_unit_test(test_insert_at_end),
    cmocka_unit_test(test_insert_at_position),
    cmocka_unit_test(test_remove_at_beginning),
    cmocka_unit_test(test_remove_at_end),
    cmocka_unit_test(test_remove_at_position),
    cmocka_unit_test(test_size),
    cmocka_unit_test(test_destroy),

/*    cmocka_unit_test(test_real_roots),
    cmocka_unit_test(test_imaginary_roots),
    cmocka_unit_test(test_complex_roots_skip),
    cmocka_unit_test(test_complex_roots_xfail),*/
  };

  return cmocka_run_group_tests(unit_tests, NULL, NULL);
}