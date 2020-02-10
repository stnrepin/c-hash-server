#include <stdlib.h>
#include <stdbool.h>

#include "unity.h"

#include "common-test-util.h"

#include "http-server.h"
#include "hash-server-error.h"

SET_UP_STUB();
TEAR_DOWN_STUB();

error_t test_route(error_t err, Request *req, Response *res) {
    if (FAIL(err)) {
        return err;
    }
    TEST_ASSERT_GREATER_OR_EQUAL(0, req->data_size);
    TEST_ASSERT_EQUAL_STRING("/api/test", req->path);
    if (req->cont_type == CONTENT_TYPE_APPLICATION_JSON) {
        TEST_ASSERT_EQUAL_STRING("{\"key\": \"data\"}", req->data);
        Response_send(res, "data", 15);
        Response_set_content_type(res, CONTENT_TYPE_APPLICATION_JSON);
    }
    else {
        TEST_FAIL();
    }
    return SUCCESS;
}

error_t test_error_route(error_t err, Request *req, Response *res) {
    if (FAIL(err)) {
        return err;
    }

    return E_ALLOC;
}

error_t test_404_route(error_t err, Request *req, Response *res) {
    if (FAIL(err)) {
        return err;
    }
    TEST_ASSERT_NOT_EQUAL("/api/test", req->path);
    Response_end(res, HTTP_CODE_NOT_FOUND);
    return SUCCESS;
}

error_t test_500_route(error_t err, Request *req, Response *res) {
    TEST_ASSERT_TRUE(FAIL(err));
    Response_end(res, HTTP_CODE_INTERNAL_ERROR);
    return SUCCESS;
}

HttpServer build_test_server() {
    HttpServer srv;
    HttpServer_init(&srv);
    HttpServer_post(&srv, "/api/test", test_route);
    HttpServer_post(&srv, "/api/test-error", test_error_route);
    HttpServer_use(&srv, test_404_route);
    HttpServer_use(&srv, test_500_route);
    return srv;
}

void should_init_valid_server() {
    HttpServer srv;

    HttpServer_init(&srv);

    TEST_ASSERT_FALSE(srv.is_listening);
    TEST_ASSERT_EQUAL(-1, srv.serv_sock);
    TEST_ASSERT_NOT_NULL(srv.routes);
    TEST_ASSERT_EQUAL(0, srv.routes->size);

    HttpServer_deinit(&srv);
}

void should_add_method_handlers() {
    HttpServer srv;
    HttpServer_init(&srv);

    HttpServer_post(&srv, "/api/test", test_route);
    //TEST_ASSERT_EQUAL(1, srv.routes->size);
    TEST_ASSERT_EQUAL(METHOD_POST, srv.routes->head->route->req_meth);
    TEST_ASSERT_EQUAL_STRING("/api/test", srv.routes->head->route->path);
    TEST_ASSERT_EQUAL(test_route, srv.routes->head->route->rc);

    HttpServer_post(&srv, "/api/test-null", NULL);
    TEST_ASSERT_EQUAL(2, srv.routes->size);
    TEST_ASSERT_EQUAL_STRING("/api/test-null", srv.routes->head->next->route->path);
    TEST_ASSERT_EQUAL(NULL, srv.routes->head->next->route->rc);

    HttpServer_deinit(&srv);
}

void should_add_generic_handlers() {
    HttpServer srv;
    HttpServer_init(&srv);

    HttpServer_use(&srv, test_404_route);
    TEST_ASSERT_EQUAL(1, srv.routes->size);
    TEST_ASSERT_EQUAL(METHOD_ALL, srv.routes->head->route->req_meth);
    TEST_ASSERT_EQUAL_STRING("*", srv.routes->head->route->path);
    TEST_ASSERT_EQUAL(test_404_route, srv.routes->head->route->rc);

    HttpServer_use(&srv, test_500_route);
    TEST_ASSERT_EQUAL(2, srv.routes->size);
    TEST_ASSERT_EQUAL(METHOD_ALL, srv.routes->head->next->route->req_meth);
    TEST_ASSERT_EQUAL_STRING("*", srv.routes->head->next->route->path);
    TEST_ASSERT_EQUAL(test_500_route, srv.routes->head->next->route->rc);

    HttpServer_deinit(&srv);
}

void should_handle_basic_request() {
    error_t err;
    HttpServer srv;
    Request req;
    Response res;
    srv = build_test_server();
    Response_init(&res);
    Request_init_from_str(&req, "POST /api/test HTTP/1.0\r\n" \
                                "Content-Type: application/json\r\n" \
                                "\r\n" \
                                "{\"key\": \"data\"}");

    err = HttpServer_handle_request(&srv, SUCCESS, &req, &res);

    TEST_ASSERT_TRUE(SUCC(err));
    TEST_ASSERT_EQUAL(true, res.is_end);
    TEST_ASSERT_EQUAL(HTTP_CODE_OK, res.code);
    TEST_ASSERT_EQUAL(CONTENT_TYPE_APPLICATION_JSON, res.cont_type);
    TEST_ASSERT_EQUAL_STRING("data", res.data);
    TEST_ASSERT_EQUAL(15, res.data_size);

    Request_deinit(&req);
    Response_deinit(&res);
    HttpServer_deinit(&srv);
}

void should_handle_not_found_request() {
    error_t err;
    HttpServer srv;
    Request req;
    Response res;
    srv = build_test_server();
    Response_init(&res);
    Request_init_from_str(&req, "POST not-existing/ HTTP/1.0\r\n" \
                                "Content-Type: application/json\r\n" \
                                "\r\n" \
                                "{\"key\": \"data\"}");

    err = HttpServer_handle_request(&srv, SUCCESS, &req, &res);

    TEST_ASSERT_TRUE(SUCC(err));
    TEST_ASSERT_EQUAL(true, res.is_end);
    TEST_ASSERT_EQUAL(HTTP_CODE_NOT_FOUND, res.code);

    Request_deinit(&req);
    Response_deinit(&res);
    HttpServer_deinit(&srv);
}

void should_handle_error_during_processing() {
    error_t err;
    HttpServer srv;
    Request req;
    Response res;
    srv = build_test_server();
    Response_init(&res);
    Request_init_from_str(&req, "POST /api/test-error HTTP/1.0\r\n" \
                                "Content-Type: application/json\r\n" \
                                "\r\n" \
                                "{\"key\": \"data\"}");

    err = HttpServer_handle_request(&srv, E_ALLOC, &req, &res);

    TEST_ASSERT_TRUE(SUCC(err));
    TEST_ASSERT_EQUAL(true, res.is_end);
    TEST_ASSERT_EQUAL(HTTP_CODE_INTERNAL_ERROR, res.code);

    err = HttpServer_handle_request(&srv, SUCCESS, &req, &res);

    TEST_ASSERT_TRUE(SUCC(err));
    TEST_ASSERT_EQUAL(true, res.is_end);
    TEST_ASSERT_EQUAL(HTTP_CODE_INTERNAL_ERROR, res.code);

    Request_deinit(&req);
    Response_deinit(&res);
    HttpServer_deinit(&srv);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(should_init_valid_server);
    RUN_TEST(should_add_method_handlers);
    RUN_TEST(should_add_generic_handlers);
    RUN_TEST(should_handle_basic_request);
    RUN_TEST(should_handle_not_found_request);
    RUN_TEST(should_handle_error_during_processing);

    return UNITY_END();
}