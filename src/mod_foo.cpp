#include <string>
#include <vector> 
#include <map>
#include <regex>
#include <algorithm>
#include <sstream>

#include <boost/process.hpp> 

#include "mod_foo.hpp"

using   String = std::string;
using   SubstitutionDict = std::map<String, String>;
using   std::regex;

namespace bp = ::boost::process; 

namespace {
  String const  gResponseTemplate (
R"(
  uri:      ${URI}
  Command:  ${COMMAND}
  Result:   ${RESULT}
)");

  String const  gKeyPrefix = R"(\$\{)";
  String const  gKeySuffix = R"(\})";
  regex         gSubstRE (R"(\$\{([^}]+)\})");

  String substitute(String const &str, SubstitutionDict const &dict, request_rec* req) {
    using std::regex_search;
    using std::regex_replace;
    using std::smatch;

    String        result = str;
    smatch        match;

    while (regex_search (result, match, gSubstRE)) {
      String      varName = match[1];
      auto const  it = dict.find(varName);

      if (it != dict.end()) {
        String    key = gKeyPrefix + varName + gKeySuffix;
        result = regex_replace(result, regex (key), it->second);
      }
      else {
        LOG_ERROR(req, "substitute: unknown '%s' variable", varName.c_str());
        break;
      }
    }

    return result;
  }

  String execute(String const &command, request_rec* req) {
    using   Arguments = std::vector<std::string>;

    std::stringstream result;

    LOG_DEBUG(req, "try to execute '%s'", command.c_str());
    try {
      String          cmd = bp::find_executable_in_path(command);
      Arguments       args; 
      bp::context     ctx; 

      LOG_DEBUG(req, "'%s' -> '%s'", command.c_str(), cmd.c_str());
      args.push_back(cmd); 

      ctx.stdout_behavior = bp::capture_stream(); 
      ctx.environment = bp::self::get_environment(); 

      bp::child       child = bp::launch(cmd, args, ctx);
      bp::pistream  & is = child.get_stdout(); 
      String          line; 

      while (std::getline(is, line)) 
        result << line << std::endl; 

      LOG_DEBUG(req, "'%s' succeeded", command.c_str());
    }
    catch (std::exception const &ex) {
      result << "exception (" << typeid(ex).name() << "): " << ex.what();
      LOG_ERROR(req, "'%s' failed - %s: %s", command.c_str(), typeid(ex).name(), ex.what());
    }

    return result.str();
  }
}


EXTERN_C_FUNC
int foo_handler( request_rec* req )
{
    int nReturnVal = DECLINED;
	
    LOG_DEBUG(req, "foo_handler: req->handler='%s', unparsed_uri='%s', uri='%s'", req->handler, req->unparsed_uri, req->uri);

    if ( req->handler != NULL && strcmp( req->handler, "foo_vc" ) == 0 ) {

      if (req->header_only) 
        return OK;

      String            uri (req->uri);
      size_t            addLength = 2;  //two slashes around prefix
      String            cmd = uri.substr(strlen(req->handler) + addLength);
      SubstitutionDict  substs = {{"URI", uri}, {"COMMAND", cmd}};

      substs["RESULT"] = execute (cmd, req);

      String            answer = substitute(gResponseTemplate, substs, req);

      ap_rputs( answer.c_str(), req );

      nReturnVal = OK;
    }

    LOG_DEBUG(req, "foo_handler: nReturnVal=%d", nReturnVal);
    return nReturnVal;
}

EXTERN_C_FUNC
void foo_hooks( apr_pool_t* inpPool )
{
    ap_hook_handler( foo_handler, NULL, NULL, APR_HOOK_MIDDLE );
}

EXTERN_C_BLOCK_BEGIN
module AP_MODULE_DECLARE_DATA foo_vc_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    foo_hooks
};
EXTERN_C_BLOCK_END
        
