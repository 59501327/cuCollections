/*
 * Copyright (c) 2021, NVIDIA CORPORATION.
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

#pragma once

#include <limits>
#include <random>

enum class dist_type { GEOMETRIC, GAUSSIAN, UNIFORM };

template <dist_type Dist, std::size_t Multiplicity, typename Key, typename OutputIt>
static void generate_keys(OutputIt output_begin, OutputIt output_end)
{
  auto const num_keys = std::distance(output_begin, output_end);

  std::random_device rd;
  std::mt19937 gen{rd()};

  switch (Dist) {
    case dist_type::GAUSSIAN: {
      auto const max  = std::numeric_limits<Key>::max();
      auto const mean = max / Multiplicity / 2;
      auto const dev  = max / Multiplicity / 5;

      std::normal_distribution<> distribution(mean, dev);

      for (auto i = 0; i < num_keys; ++i) {
        output_begin[i] = std::abs(distribution(gen));
      }
      break;
    }
    case dist_type::GEOMETRIC: {
      std::geometric_distribution<Key> distribution{1e-9};

      for (auto i = 0; i < num_keys; ++i) {
        output_begin[i] = distribution(gen) / Multiplicity;
      }
      break;
    }
    case dist_type::UNIFORM: {
      std::uniform_int_distribution<Key> distribution{1, static_cast<Key>(num_keys / Multiplicity)};

      for (auto i = 0; i < num_keys; ++i) {
        output_begin[i] = distribution(gen);
      }
      break;
    }
  }  // switch
}

template <std::size_t Multiplicity, typename Key, typename OutputIt>
static void generate_prob_keys(OutputIt output_end,
                               OutputIt output_begin,
                               double const matching_rate)
{
  auto const num_keys = std::distance(output_begin, output_end);

  std::random_device rd;
  std::mt19937 gen{rd()};

  auto const max = std::numeric_limits<Key>::max();

  std::uniform_real_distribution<double> rate_dist(0.0, 1.0);
  std::uniform_int_distribution<Key> non_match_dist{max / Multiplicity, max};

  for (auto i = 0; i < num_keys; ++i) {
    auto const tmp_rate = rate_dist(gen);

    // If non-match, randomly sample from [Max/Multiplicity, Max]
    if (tmp_rate > matching_rate) { output_begin[i] = non_match_dist(gen); }
  }
}
