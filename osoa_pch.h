#if defined(_MSC_VER) && _MSC_VER >= 1400 

#pragma warning(disable:4512)
#pragma warning(disable:4996)
#pragma warning(disable:4510)
#pragma warning(disable:4610)
#pragma warning(disable:4100)

#include <algorithm>
#include <cmath>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/array.hpp"
#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include "boost/chrono.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/log/attributes/current_process_name.hpp"
#include "boost/log/core.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/sinks.hpp"
#include "boost/log/support/date_time.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"
#include "boost/log/utility/setup/console.hpp"
#include "boost/noncopyable.hpp"
#include "boost/optional.hpp"
#include "boost/program_options.hpp"
#include "boost/shared_ptr.hpp"

#include "Poco/Format.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Util/ServerApplication.h"

#pragma warning(default:4512)
#pragma warning(default:4996)
#pragma warning(default:4510  )
#pragma warning(default:4610)
#pragma warning(default:4100)
#endif
