#include <check.h>
#include <pod.h>



START_TEST(test_get_pod_mode)
{
    int rc = init_pod();
    ck_assert_int_eq(rc, 0);
    ck_assert_int_eq(get_pod_mode(), POST);

    ck_assert_int_eq(set_pod_mode(Boot, "Test Set Mode"), true);
    ck_assert_str_eq(get_pod()->state_reason, "Test Set Mode");
    uint64_t time = get_pod()->last_transition;

    ck_assert_int_eq(set_pod_mode(Boot, "Test Set Mode"), false);
    ck_assert_str_eq(get_pod()->state_reason, "Test Set Mode");

    ck_assert_int_eq(get_pod()->last_transition, time);
}
END_TEST

int main(void)
{
    return 0;
}