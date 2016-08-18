
CXXFLAGS =  --std=c++11
CXXFLAGS += -Ihttp/include
CXXFLAGS += -I.
CXXFLAGS += -g
CXXFLAGS += -Iprocess

all: service


OBJECTS=service.o manager.o python.o
LIBS=-lboost_system -lboost_coroutine -lpthread -lboost_python -lpython2.7

service: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o service ${LIBS}

depend:
	makedepend -Yhttp/include -I. *.C


# DO NOT DELETE

manager.o: manager.h
python.o: manager.h
service.o: http/include/boost/http/buffered_socket.hpp
service.o: http/include/boost/http/socket.hpp
service.o: http/include/boost/http/reader/request.hpp
service.o: http/include/boost/http/syntax/chunk_size.hpp
service.o: http/include/boost/http/syntax/chunk_size.ipp
service.o: http/include/boost/http/syntax/content_length.hpp
service.o: http/include/boost/http/syntax/content_length.ipp
service.o: http/include/boost/http/syntax/crlf.hpp
service.o: http/include/boost/http/syntax/crlf.ipp
service.o: http/include/boost/http/syntax/ows.hpp
service.o: http/include/boost/http/syntax/detail/is_ows.hpp
service.o: http/include/boost/http/syntax/ows.ipp
service.o: http/include/boost/http/syntax/field_name.hpp
service.o: http/include/boost/http/syntax/field_name.ipp
service.o: http/include/boost/http/syntax/field_value.hpp
service.o: http/include/boost/http/syntax/detail/is_vchar.hpp
service.o: http/include/boost/http/syntax/detail/is_obs_text.hpp
service.o: http/include/boost/http/syntax/field_value.ipp
service.o: http/include/boost/http/detail/macros.hpp
service.o: http/include/boost/http/reader/detail/transfer_encoding.hpp
service.o: http/include/boost/http/algorithm/header/header_value_any_of.hpp
service.o: http/include/boost/http/reader/detail/abnf.hpp
service.o: http/include/boost/http/reader/detail/common.hpp
service.o: http/include/boost/http/token.hpp
service.o: http/include/boost/http/reader/request.ipp
service.o: http/include/boost/http/traits.hpp
service.o: http/include/boost/http/read_state.hpp
service.o: http/include/boost/http/write_state.hpp
service.o: http/include/boost/http/message.hpp
service.o: http/include/boost/http/headers.hpp
service.o: http/include/boost/http/message-inl.hpp
service.o: http/include/boost/http/http_errc.hpp
service.o: http/include/boost/http/detail/singleton.hpp
service.o: http/include/boost/http/detail/writer_helper.hpp
service.o: http/include/boost/http/detail/constchar_helper.hpp
service.o: http/include/boost/http/algorithm/header.hpp
service.o: http/include/boost/http/socket-inl.hpp
service.o: http/include/boost/http/algorithm.hpp
service.o: http/include/boost/http/algorithm/query.hpp
service.o: http/include/boost/http/algorithm/write.hpp
service.o: http/include/boost/http/status_code.hpp
service.o: http/include/boost/http/status_code-inl.hpp manager.h
