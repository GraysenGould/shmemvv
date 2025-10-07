/**
 * @file c11_shmem_atomic_inc.c
 * @brief Unit test for shmem_atomic_inc
 */

#include <shmem.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "shmemvv.h"
#include "type_tables.h"

#define TEST_C11_SHMEM_ATOMIC_INC(TYPE)                                        \
  ({                                                                           \
    log_routine("shmem_atomic_inc(" #TYPE ")");                                \
    bool success = true;                                                       \
    static TYPE *dest;                                                         \
    dest = (TYPE *)shmem_malloc(sizeof(TYPE));                                 \
    log_info("shmem_malloc'd %d bytes at %p", sizeof(TYPE), (void *)dest);     \
    TYPE value = 42;                                                           \
    *dest = value;                                                             \
    log_info("initialized dest at %p to %d", (void *)dest, (int)value);        \
    shmem_barrier_all();                                                       \
    int mype = shmem_my_pe();                                                  \
    int npes = shmem_n_pes();                                                  \
    log_info("executing atomic inc: dest = %p", (void *)dest);                 \
    shmem_atomic_inc(dest, (mype + 1) % npes);                                 \
    shmem_barrier_all();                                                       \
    success = (*dest == value + 1);                                            \
    if (!success)                                                              \
      log_fail("atomic inc on %s did not produce expected value %d, "          \
               "got instead %d",                                               \
               #TYPE, (int)(value + 1), (int)*dest);                           \
    else                                                                       \
      log_info("atomic inc on a %s at %p produced expected result "            \
               "(%d + 1 = %d)",                                                \
               #TYPE, (void *)dest, (int)value, (int)*dest);                   \
    shmem_free(dest);                                                          \
    success;                                                                   \
  })

#define ATOMIC_TEST_C11_SHMEM_ATOMIC_INC(TYPE)                                 \
  ({                                                                           \
    log_routine("shmem_atomic_inc(" #TYPE ") atomic test");                    \
    bool success = true;                                                       \
    int my_pe, npes;                                                           \
    npes = shmem_n_pes();                                                      \
    my_pe = shmem_my_pe();                                                     \
    int count = 100000;                                                        \
    static TYPE *counter;                                                      \
    counter = (TYPE *)shmem_malloc(sizeof(TYPE));                              \
    *counter = 0;                                                              \
    log_info("shmem_malloc'd %d bytes at %p", sizeof(TYPE), (void *)counter);  \
    shmem_barrier_all();                                                       \
    log_info("PE %d: Executing atomic increments",my_pe);                      \
    for (int i = 0; i < count; i ++){                                          \
        shmem_atomic_inc(counter, 0);                                          \
    }                                                                          \
    shmem_quiet();                                                             \
    shmem_barrier_all();                                                       \
    if (my_pe == 0){                                                           \
        log_info("PE 0: Validating incrementing results");                     \
        log_info("Expected count: %d, Actual: %d", (npes  * count), *counter); \
        if (*counter == (npes * count)){                                       \
            log_info("PE 0: Count is correct: %d",*counter);                   \
        }                                                                      \
        else{                                                                  \
            log_fail("PE 0: Count is not correct: %d", *counter);              \
            success = 0;                                                       \
        }                                                                      \
    }                                                                          \
    shmem_free(counter);                                                       \
    success;                                                                   \
  })

#define TEST_C11_CTX_SHMEM_ATOMIC_INC(TYPE)                                    \
  ({                                                                           \
    log_routine("shmem_atomic_inc(ctx, " #TYPE ")");                           \
    bool success = true;                                                       \
    static TYPE *dest;                                                         \
    dest = (TYPE *)shmem_malloc(sizeof(TYPE));                                 \
    log_info("shmem_malloc'd %d bytes at %p", sizeof(TYPE), (void *)dest);     \
    TYPE value = 42;                                                           \
    *dest = value;                                                             \
    log_info("initialized dest at %p to %d", (void *)dest, (int)value);        \
                                                                               \
    shmem_ctx_t ctx;                                                           \
    int ctx_create_status = shmem_ctx_create(0, &ctx);                         \
    if (ctx_create_status != 0) {                                              \
      log_fail("Failed to create context");                                    \
      shmem_free(dest);                                                        \
      return false;                                                            \
    }                                                                          \
    log_info("Successfully created context");                                  \
                                                                               \
    shmem_barrier_all();                                                       \
    int mype = shmem_my_pe();                                                  \
    int npes = shmem_n_pes();                                                  \
    log_info("executing atomic inc with context: dest = %p", (void *)dest);    \
    shmem_atomic_inc(ctx, dest, (mype + 1) % npes);                            \
    shmem_ctx_quiet(ctx);                                                      \
    shmem_barrier_all();                                                       \
    success = (*dest == value + 1);                                            \
    if (!success)                                                              \
      log_fail("atomic inc with context on %s did not produce expected "       \
               "value %d, got instead %d",                                     \
               #TYPE, (int)(value + 1), (int)*dest);                           \
    else                                                                       \
      log_info("atomic inc with context on a %s at %p produced expected "      \
               "result (%d + 1 = %d)",                                         \
               #TYPE, (void *)dest, (int)value, (int)*dest);                   \
                                                                               \
    shmem_ctx_destroy(ctx);                                                    \
    log_info("Context destroyed");                                             \
    shmem_free(dest);                                                          \
    success;                                                                   \
  })

  #define ATOMIC_TEST_C11_CTX_SHMEM_ATOMIC_INC(TYPE)                           \
  ({                                                                           \
    log_routine("shmem_atomic_inc(ctx, " #TYPE ") atomic test");               \
    bool success = true;                                                       \
    int my_pe, npes;                                                           \
    npes = shmem_n_pes();                                                      \
    my_pe = shmem_my_pe();                                                     \
    int count = 100000;                                                        \
    static TYPE *counter;                                                      \
    counter = (TYPE *)shmem_malloc(sizeof(TYPE));                              \
    *counter = 0;                                                              \
    log_info("shmem_malloc'd %d bytes at %p", sizeof(TYPE), (void *)counter);  \
    shmem_ctx_t ctx;                                                           \
    int ctx_create_status = shmem_ctx_create(0, &ctx);                         \
    if (ctx_create_status != 0) {                                              \
      log_fail("Failed to create context");                                    \
      shmem_free(counter);                                                     \
      return false;                                                            \
    }                                                                          \
    log_info("Successfully created context");                                  \
    shmem_barrier_all();                                                       \
    log_info("PE %d: Executing atomic increments",my_pe);                      \
    for (int i = 0; i < count; i ++){                                          \
        shmem_atomic_inc(ctx, counter, 0);                                     \
    }                                                                          \
    shmem_ctx_quiet(ctx);                                                      \
    shmem_barrier_all();                                                       \
    if (my_pe == 0){                                                           \
        log_info("PE 0: Validating incrementing results");                     \
        log_info("Expected count: %d, Actual: %d", (npes * count), *counter);  \
        if (*counter == (npes * count)){                                       \
            log_info("PE 0: Count is correct: %d",*counter);                   \
        }                                                                      \
        else{                                                                  \
            log_fail("PE 0: Count is not correct: %d", *counter);              \
            success = 0;                                                       \
        }                                                                      \
    }                                                                          \
    shmem_ctx_destroy(ctx);                                                    \
    log_info("Context destroyed");                                             \
    shmem_free(counter);                                                       \
    success;                                                                   \
  })

int main(int argc, char *argv[]) {
  shmem_init();
  log_init(__FILE__);

  int rc = EXIT_SUCCESS;

  /* Test standard atomic inc operations */
  bool result = true;

  #define X(type, shmem_types) result &= TEST_C11_SHMEM_ATOMIC_INC(type);
    SHMEM_STANDARD_AMO_TYPE_TABLE(X)
  #undef X

  if (shmem_my_pe() == 0) {
    display_test_result("C11 shmem_atomic_inc", result, false);
  }

  /* Test atomic guarentee of standard routine */
  bool result_atomic = true;
  #define X(type, shmem_types) result_atomic &= ATOMIC_TEST_C11_SHMEM_ATOMIC_INC(type);
    SHMEM_STANDARD_AMO_TYPE_TABLE(X)
  #undef X

  if (shmem_my_pe() == 0) {
    display_test_result("C11 shmem_atomic_inc atomic test", result_atomic, false);
  }

  /* Test context-specific atomic inc operations */
  bool result_ctx = true;

  #define X(type, shmem_types) result_ctx &= TEST_C11_CTX_SHMEM_ATOMIC_INC(type);
    SHMEM_STANDARD_AMO_TYPE_TABLE(X)
  #undef X

  if (shmem_my_pe() == 0) {
    display_test_result("C11 shmem_atomic_inc with ctx", result_ctx, false);
  }

  /* Test atomics context-specific atomic inc operations */
  bool result_ctx_atomic = true;

  #define X(type, shmem_types) result_ctx_atomic &= ATOMIC_TEST_C11_CTX_SHMEM_ATOMIC_INC(type);
    SHMEM_STANDARD_AMO_TYPE_TABLE(X)
  #undef X

  if (shmem_my_pe() == 0) {
    display_test_result("C11 shmem_atomic_inc atomic test with ctx", result_ctx_atomic, false);
  }

  if (!result || !result_ctx) {
    rc = EXIT_FAILURE;
  }

  log_close(rc);
  shmem_finalize();
  return rc;
}
