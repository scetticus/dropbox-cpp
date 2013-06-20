#ifndef __DROPBOX_METADATA_TYPE_H__
#define __DROPBOX_METADATA_TYPE_H__

#include "DropboxException.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sys/types.h>
#include <string>

namespace dropbox {

const size_t DEFAULT_FILE_LIMIT = 10;

typedef struct DropboxMetadata {
  std::string         path_;
  std::string         sizeStr_;
  size_t              sizeBytes_;
  bool                isDir_;
  std::string         mimeType_;
  bool                isDeleted_;
  std::string         rev_;
  std::string         hash_;
  bool                thumbExists_;
  std::string         icon_;
  std::string         clientMtime_;
  std::string         root_;

  static void readFromJson(boost::property_tree::ptree& pt,
      DropboxMetadata& m) {
    using namespace boost::property_tree;
    using namespace boost::property_tree::json_parser;
    using namespace std;

    try {
      // Mandatory fields
      m.path_ = pt.get<string>("path");
      m.sizeStr_ = pt.get<string>("size");
      m.sizeBytes_ = pt.get<size_t>("bytes");
      m.icon_ = pt.get<string>("icon");
      m.root_ = pt.get<string>("root");

      // Optional fields
      m.rev_ = pt.get<string>("rev", "");
      m.hash_ = pt.get<string>("hash", "");
      m.clientMtime_ = pt.get<string>("client_mtime", "");

      auto boolParser = [&](string field, string defaultVal, bool& val) {
        string strval = pt.get<string>(field, defaultVal);
        if (strval.compare("true") == 0) {
          val = true;
        } else if (strval.compare("false") == 0) {
          val = false;
        } else {
          stringstream s;
          s << "Bad '" << field << "' value: " << strval;
          throw DropboxException(MALFORMED_RESPONSE, s.str());
        }
      };

      boolParser("is_dir", "false", m.isDir_);
      boolParser("is_deleted", "false", m.isDeleted_);
      boolParser("thumb_exists", "false", m.thumbExists_);

      m.mimeType_ = pt.get<string>("mime_type", "");
    } catch (exception& e) {
      throw DropboxException(MALFORMED_RESPONSE, e.what());
    }
  }
} DropboxMetadata;
}
#endif
