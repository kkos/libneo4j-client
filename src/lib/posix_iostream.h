/* vi:set ts=4 sw=4 expandtab:
 *
 * Copyright 2016, Chris Leishman (http://github.com/cleishm)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef NEO4J_POSIX_IOSTREAM_H
#define NEO4J_POSIX_IOSTREAM_H

#include "neo4j-client.h"
#include "iostream.h"

/**
 * Create an iostream for a POSIX file descriptor.
 *
 * @internal
 *
 * @param [fd] The file descriptor to create an iostream for.
 * @return The newly created iostream.
 */
__neo4j_must_check
neo4j_iostream_t *neo4j_posix_iostream(int fd);

#endif/*NEO4J_POSIX_IOSTREAM_H*/
