/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef IROHA_IROHA_INSTANCE_HPP
#define IROHA_IROHA_INSTANCE_HPP

#include <cstdlib>
#include "integration/pipeline/tx_pipeline_integration_test_fixture.hpp"
#include "main/raw_block_insertion.hpp"

namespace integration_framework {

  using namespace std::chrono_literals;

  class IrohaInstance {
   public:
    void clearLedger() {
      instance_->storage->dropStorage();
    }

    void rawInsertBlock(const iroha::model::Block &block) {
      iroha::main::BlockInserter inserter(instance_->storage);
      inserter.applyToLedger({block});
    }

    void initPipeline(const iroha::keypair_t &key_pair) {
      keypair_ = key_pair;
      instance_ = std::make_shared<TestIrohad>(block_store_dir_,
                                               redis_host_,
                                               redis_port_,
                                               pg_conn_,
                                               torii_port_,
                                               internal_port_,
                                               max_proposal_size_,
                                               proposal_delay_,
                                               vote_delay_,
                                               load_delay_,
                                               keypair_);
      instance_->init();
    }

    void run() {
      instance_->run();
    }

    // TODO 20/12/2017 muratovv replace with auto return type
    std::shared_ptr<TestIrohad> getIrohaInstance() {
      return instance_;
    }

    std::shared_ptr<TestIrohad> instance_;

    std::string getPostgreCreds() {
      auto pg_host = std::getenv("IROHA_POSTGRES_HOST");
      auto pg_port = std::getenv("IROHA_POSTGRES_PORT");
      auto pg_user = std::getenv("IROHA_POSTGRES_USER");
      auto pg_pass = std::getenv("IROHA_POSTGRES_PASSWORD");
      if (not pg_host) {
        return "host=localhost port=5432 user=postgres "
               "password=mysecretpassword";
      } else {
        std::stringstream ss;
        ss << "host=" << pg_host << " port=" << pg_port << " user=" << pg_user
           << " password=" << pg_pass;
        return ss.str();
      }
    }

    // config area
    const std::string block_store_dir_ = "/tmp/block_store";
    const std::string redis_host_ = std::getenv("IROHA_REDIS_HOST");
    const size_t redis_port_ = std::stoull(std::getenv("IROHA_REDIS_PORT"));
    const std::string pg_conn_ = getPostgreCreds();
    const size_t torii_port_ = 11501;
    const size_t internal_port_ = 10001;
    const size_t max_proposal_size_ = 10;
    const std::chrono::milliseconds proposal_delay_ = 5000ms;
    const std::chrono::milliseconds vote_delay_ = 5000ms;
    const std::chrono::milliseconds load_delay_ = 5000ms;
    iroha::keypair_t keypair_;
  };
}  // namespace integration_framework
#endif  // IROHA_IROHA_INSTANCE_HPP