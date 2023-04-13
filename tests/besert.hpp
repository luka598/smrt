/*
 * Copyright 2023 luka598 <luka598@tuta.io>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

// BESERT_BEGIN - Prints to cout when assertion begins
// BESERT_FATAL - Throw runtime error when assertion fails
// BESERT_NONFATAL - Print to cout error when assertion fails
// BESERT_END - Print to cout result of assertion
// BESERT_INCREMENT_SUCCESS - Increments n on success
// BESERT_INCREMENT_FAIL - Incremenets n on fail
// BESERT_BORDERS - Add borders on begin and end of assertion
// TODO: BESERT_MSG_FILE - Include __FILE__ in logs
// TODO: BESERT_MSG_LINE - Include __LINE__ in logs
// TODO: BESERT_MSG_FUNC - Include __func__ in logs

#include <iostream>
#include <stdexcept>

#define LOG_STR(message)                                                       \
  (std::string(__FILE__) + ":" + std::to_string(__LINE__) + " (" +             \
   std::string(__func__) + "): " + message)

// I might move everyting to macros, don't know if it is good idea yet.
namespace besert {
void inline begin(std::string assertion) {
#ifdef BESERT_BORDERS
  std::cout << std::string(10, '-') << std::endl;
#endif
#ifdef BESERT_BEGIN
  std::cout << LOG_STR("Asserting " + assertion) << std::endl;
#endif
}

void inline end(bool result) {
#ifdef BESERT_END
  std::cout << LOG_STR("Assertion result " + std::to_string(result))
            << std::endl;
#endif

#ifdef BESERT_BORDERS
  std::cout << std::string(10, '-') << std::endl;
#endif
}

void inline end(bool result, int *n) {
#ifdef BESERT_END
  std::cout << LOG_STR("Assertion result " + std::to_string(result))
            << std::endl;
#endif

#ifdef BESERT_INCREMENT_SUCCESS
  if (result)
    *n += 1;
#endif

#ifdef BESERT_INCREMENT_FAIL
  if (!result)
    *n += 1;
#endif

#ifdef BESERT_BORDERS
  std::cout << std::string(10, '-') << std::endl;
#endif
}

bool inline assert(bool result) {
  if (result)
    return true;

#ifdef BESERT_FATAL
  throw std::runtime_error(LOG_STR("Assertion fail"));
#endif

#ifdef BESERT_NONFATAL
  std::cout << LOG_STR("Assertion fail") << std::endl;
#endif

  return false;
}

bool inline assert(bool result, std::string message) {
  if (result)
    return true;

#ifdef BESERT_FATAL
  throw std::runtime_error(LOG_STR("Assertion fail: " + message));
#endif

#ifdef BESERT_NONFATAL
  std::cout << LOG_STR("Assertion fail: " + message) << std::endl;
#endif

  return false;
}
// IDEA: Add assert with callback
} // namespace besert

// Maybe I should just make them funcitons?
#define ASSERT(assertion)                                                      \
  besert::begin(#assertion);                                                   \
  besert::assert(assertion, #assertion);                                       \
  besert::end(assertion)

#define ASSERTI(assertion, n_ptr)                                              \
  besert::begin(#assertion);                                                   \
  besert::assert(assertion, #assertion);                                       \
  besert::end(assertion, n_ptr)

#define ASSERTM(assertion, begin_message, fail_message)                        \
  besert::begin(std::string(#assertion) + "; " + std::string(begin_message));  \
  besert::assert(assertion,                                                    \
                 std::string(#assertion) + "; " + std::string(fail_message));  \
  besert::end(assertion)

#define ASSERTIM(assertion, n_ptr, begin_message, fail_message)                \
  besert::begin(std::string(#assertion) + "; " + std::string(begin_message));  \
  besert::assert(assertion,                                                    \
                 std::string(#assertion) + "; " + std::string(fail_message));  \
  besert::end(assertion, n_ptr)
