/*
 * Copyright 2004-present Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <folly/experimental/logging/LogConfig.h>

namespace folly {

bool LogConfig::operator==(const LogConfig& other) const {
  return handlerConfigs_ == other.handlerConfigs_ &&
      categoryConfigs_ == other.categoryConfigs_;
}

bool LogConfig::operator!=(const LogConfig& other) const {
  return !(*this == other);
}

void LogConfig::update(const LogConfig& other) {
  // Update handlerConfigs_ with all of the entries from the other LogConfig.
  // Any entries already present in our handlerConfigs_ are replaced wholesale.
  for (const auto& entry : other.handlerConfigs_) {
    auto result = handlerConfigs_.insert(entry);
    if (!result.second) {
      result.first->second = entry.second;
    }
  }

  // Update categoryConfigs_ with all of the entries from the other LogConfig.
  //
  // Any entries already present in our categoryConfigs_ are merged: if the new
  // configuration does not include handler settings our entry's settings are
  // maintained.
  for (const auto& entry : other.categoryConfigs_) {
    auto result = categoryConfigs_.insert(entry);
    if (!result.second) {
      auto* existingEntry = &result.first->second;
      auto oldHandlers = std::move(existingEntry->handlers);
      *existingEntry = entry.second;
      if (!existingEntry->handlers.hasValue()) {
        existingEntry->handlers = std::move(oldHandlers);
      }
    }
  }
}

} // namespace folly
