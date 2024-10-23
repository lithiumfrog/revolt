#include "ut_darray.h"

#include "platform/platform.h"
#include "containers/darray.h"

void ut_darray_run(void)
{
    ut_darray_create_destroy();
    ut_darray_push_pop();
    ut_darray_push_pop_many();
    ut_darray_erase_any_matching();
    ut_darray_zero();
    ut_darray_resize();
}

void ut_darray_create_destroy(void)
{
    memory_pool_st* pool1;
    STANDARD_RESULT res = memory_pool_create("pool1", 1, &pool1);
    (void)res;
    STANDARD_ASSERT(res == 0);

    // first to last
    u64* darray1 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);

    u64* darray2 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);

    u64* darray3 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);


    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);
    darray_destroy(darray1);
    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);
    darray_destroy(darray2);
    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);
    darray_destroy(darray3);
    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);


    // last to first
    u64* darray4 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);

    u64* darray5 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);

    u64* darray6 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);


    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);
    darray_destroy(darray6);
    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);
    darray_destroy(darray5);
    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);
    darray_destroy(darray4);
    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);

    memory_pool_destroy(pool1);
}

void ut_darray_push_pop(void)
{
    memory_pool_st* pool1;
    STANDARD_RESULT res = memory_pool_create("pool1", 1, &pool1);
    (void)res;
    STANDARD_ASSERT(res == 0);

    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);

    u64* darray1 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);

    // capacity 1 write/pop
    u64 temp1 = darray_length_get(darray1);
    (void)temp1;
    u64 a = 1234567890;
    darray1[0] = a;
    u64 temp2 = darray_length_get(darray1);
    (void)temp2;
    darray_length_set(darray1, 1);
    u64 b = darray_pop(darray1);
    (void)b;
    STANDARD_ASSERT(a == b);

    // capacity 1 push/read
    u64 c = 9876543210;
    darray_push(darray1, c);
    u64 d = darray1[0];
    (void)d;
    STANDARD_ASSERT(c == d);
        // manually remove this entry to reset the darray
        darray_length_set(darray1, 0);
        // check length
        STANDARD_ASSERT(darray_length_get(darray1) == 0)

    // capacity 1 push/pop
    u64 e = 123454321;
    darray_push(darray1, e);
    u64 f = darray_pop(darray1);
    (void)f;
    STANDARD_ASSERT(e == f);

    // cleanup
    darray_destroy(darray1);

    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);

    memory_pool_destroy(pool1);
}

void ut_darray_push_pop_many(void)
{
    memory_pool_st* pool1;
    STANDARD_RESULT res = memory_pool_create("pool1", 1, &pool1);
    (void)res;
    STANDARD_ASSERT(res == 0);

    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);

    u64* darray1 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);

    // push 10 pop 10
    u64 aa = 1;
    u64 ab = 2;
    u64 ac = 3;
    u64 ad = 4;
    u64 ae = 5;
    u64 af = 6;
    u64 ag = 7;
    u64 ah = 8;
    u64 ai = 9;
    u64 aj = 10;
    darray_push(darray1, aa);
    darray_push(darray1, ab);
    darray_push(darray1, ac);
    darray_push(darray1, ad);
    darray_push(darray1, ae);
    darray_push(darray1, af);
    darray_push(darray1, ag);
    darray_push(darray1, ah);
    darray_push(darray1, ai);
    darray_push(darray1, aj);
    u64 aa_o = darray_pop(darray1);
    u64 ab_o = darray_pop(darray1);
    u64 ac_o = darray_pop(darray1);
    u64 ad_o = darray_pop(darray1);
    u64 ae_o = darray_pop(darray1);
    u64 af_o = darray_pop(darray1);
    u64 ag_o = darray_pop(darray1);
    u64 ah_o = darray_pop(darray1);
    u64 ai_o = darray_pop(darray1);
    u64 aj_o = darray_pop(darray1);
    (void)aa_o;
    (void)ab_o;
    (void)ac_o;
    (void)ad_o;
    (void)ae_o;
    (void)af_o;
    (void)ag_o;
    (void)ah_o;
    (void)ai_o;
    (void)aj_o;
    STANDARD_ASSERT(aa == aj_o);
    STANDARD_ASSERT(ab == ai_o);
    STANDARD_ASSERT(ac == ah_o);
    STANDARD_ASSERT(ad == ag_o);
    STANDARD_ASSERT(ae == af_o);
    STANDARD_ASSERT(af == ae_o);
    STANDARD_ASSERT(ag == ad_o);
    STANDARD_ASSERT(ah == ac_o);
    STANDARD_ASSERT(ai == ab_o);
    STANDARD_ASSERT(aj == aa_o);

    // push 10 pop at last to first
    u64 ba = 1;
    u64 bb = 2;
    u64 bc = 3;
    u64 bd = 4;
    u64 be = 5;
    u64 bf = 6;
    u64 bg = 7;
    u64 bh = 8;
    u64 bi = 9;
    u64 bj = 10;
    darray_push(darray1, ba);
    darray_push(darray1, bb);
    darray_push(darray1, bc);
    darray_push(darray1, bd);
    darray_push(darray1, be);
    darray_push(darray1, bf);
    darray_push(darray1, bg);
    darray_push(darray1, bh);
    darray_push(darray1, bi);
    darray_push(darray1, bj);
    u64 ba_o = darray_pop_at(darray1, 9);
    u64 bb_o = darray_pop_at(darray1, 8);
    u64 bc_o = darray_pop_at(darray1, 7);
    u64 bd_o = darray_pop_at(darray1, 6);
    u64 be_o = darray_pop_at(darray1, 5);
    u64 bf_o = darray_pop_at(darray1, 4);
    u64 bg_o = darray_pop_at(darray1, 3);
    u64 bh_o = darray_pop_at(darray1, 2);
    u64 bi_o = darray_pop_at(darray1, 1);
    u64 bj_o = darray_pop_at(darray1, 0);
    (void)ba_o;
    (void)bb_o;
    (void)bc_o;
    (void)bd_o;
    (void)be_o;
    (void)bf_o;
    (void)bg_o;
    (void)bh_o;
    (void)bi_o;
    (void)bj_o;
    STANDARD_ASSERT(ba == bj_o);
    STANDARD_ASSERT(bb == bi_o);
    STANDARD_ASSERT(bc == bh_o);
    STANDARD_ASSERT(bd == bg_o);
    STANDARD_ASSERT(be == bf_o);
    STANDARD_ASSERT(bf == be_o);
    STANDARD_ASSERT(bg == bd_o);
    STANDARD_ASSERT(bh == bc_o);
    STANDARD_ASSERT(bi == bb_o);
    STANDARD_ASSERT(bj == ba_o);

    // push 10 pop at first to last
    u64 ca = 1;
    u64 cb = 2;
    u64 cc = 3;
    u64 cd = 4;
    u64 ce = 5;
    u64 cf = 6;
    u64 cg = 7;
    u64 ch = 8;
    u64 ci = 9;
    u64 cj = 10;
    darray_push(darray1, ca);
    darray_push(darray1, cb);
    darray_push(darray1, cc);
    darray_push(darray1, cd);
    darray_push(darray1, ce);
    darray_push(darray1, cf);
    darray_push(darray1, cg);
    darray_push(darray1, ch);
    darray_push(darray1, ci);
    darray_push(darray1, cj);
    u64 ca_o = darray_pop_at(darray1, 0);
    u64 cb_o = darray_pop_at(darray1, 0);
    u64 cc_o = darray_pop_at(darray1, 0);
    u64 cd_o = darray_pop_at(darray1, 0);
    u64 ce_o = darray_pop_at(darray1, 0);
    u64 cf_o = darray_pop_at(darray1, 0);
    u64 cg_o = darray_pop_at(darray1, 0);
    u64 ch_o = darray_pop_at(darray1, 0);
    u64 ci_o = darray_pop_at(darray1, 0);
    u64 cj_o = darray_pop_at(darray1, 0);
    (void)ca_o;
    (void)cb_o;
    (void)cc_o;
    (void)cd_o;
    (void)ce_o;
    (void)cf_o;
    (void)cg_o;
    (void)ch_o;
    (void)ci_o;
    (void)cj_o;
    STANDARD_ASSERT(ca == ca_o);
    STANDARD_ASSERT(cb == cb_o);
    STANDARD_ASSERT(cc == cc_o);
    STANDARD_ASSERT(cd == cd_o);
    STANDARD_ASSERT(ce == ce_o);
    STANDARD_ASSERT(cf == cf_o);
    STANDARD_ASSERT(cg == cg_o);
    STANDARD_ASSERT(ch == ch_o);
    STANDARD_ASSERT(ci == ci_o);
    STANDARD_ASSERT(cj == cj_o);

    // cleanup
    darray_destroy(darray1);

    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);

    memory_pool_destroy(pool1);
}

void ut_darray_erase_any_matching(void)
{
    pools_list_print();

    memory_pool_st* pool1;
    STANDARD_RESULT res = memory_pool_create("pool1", MiB(1), &pool1);
    (void)res;
    STANDARD_ASSERT(res == 0);

    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);

    u64* darray1 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);
    u64 da = 1;
    u64 db = 2;
    u64 dc = 3;
    u64 dd = 4;
    u64 de = 5;
    u64 df = 6;
    u64 dg = 7;
    u64 dh = 8;
    u64 di = 9;
    u64 dj = 10;
    darray_push(darray1, da);
    darray_push(darray1, db);
    darray_push(darray1, dc);
    darray_push(darray1, dd);
    darray_push(darray1, de);
    darray_push(darray1, df);
    darray_push(darray1, dg);
    darray_push(darray1, dh);
    darray_push(darray1, di);
    darray_push(darray1, dj);
    STANDARD_ASSERT(darray1[0] == 1);
    STANDARD_ASSERT(darray_length_get(darray1) == 10);
    u64 check1 = 1u;
    darray_erase_any_matching(darray1, check1);
    STANDARD_ASSERT(darray1[0] == 2);
    STANDARD_ASSERT(darray1[8] == 10);
    STANDARD_ASSERT(darray_length_get(darray1) == 9);
    u64 check10 = 10u;
    darray_erase_any_matching(darray1, check10);
    STANDARD_ASSERT(darray1[8] == 10);
    STANDARD_ASSERT(darray1[9] == 10);
    STANDARD_ASSERT(darray_length_get(darray1) == 8);
    u64 check5 = 5u;
    darray_erase_any_matching(darray1, check5);
    STANDARD_ASSERT(darray1[3] == 6);
    STANDARD_ASSERT(darray_length_get(darray1) == 7);
    darray_destroy(darray1);


    // darray_erase_any_matching many in middle
    u64* darray2 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);
    u64 ea = 1;
    u64 eb = 2;
    u64 ec = 2;
    u64 ed = 2;
    u64 ee = 2;
    u64 ef = 2;
    u64 eg = 2;
    u64 eh = 2;
    u64 ei = 2;
    u64 ej = 3;
    darray_push(darray2, ea);
    darray_push(darray2, eb);
    darray_push(darray2, ec);
    darray_push(darray2, ed);
    darray_push(darray2, ee);
    darray_push(darray2, ef);
    darray_push(darray2, eg);
    darray_push(darray2, eh);
    darray_push(darray2, ei);
    darray_push(darray2, ej);
    darray_erase_any_matching(darray2, eb);
    STANDARD_ASSERT(darray_length_get(darray2) == 2);
    STANDARD_ASSERT(darray2[0] == 1);
    STANDARD_ASSERT(darray2[1] == 3);
    darray_destroy(darray2);


    // darray_erase_any_matching many at beginning
    u64* darray3 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);
    u64 fa = 1;
    u64 fb = 1;
    u64 fc = 1;
    u64 fd = 1;
    u64 fe = 1;
    u64 ff = 2;
    u64 fg = 3;
    u64 fh = 4;
    u64 fi = 5;
    u64 fj = 6;
    darray_push(darray3, fa);
    darray_push(darray3, fb);
    darray_push(darray3, fc);
    darray_push(darray3, fd);
    darray_push(darray3, fe);
    darray_push(darray3, ff);
    darray_push(darray3, fg);
    darray_push(darray3, fh);
    darray_push(darray3, fi);
    darray_push(darray3, fj);
    darray_erase_any_matching(darray3, fa);
    STANDARD_ASSERT(darray_length_get(darray3) == 5);
    STANDARD_ASSERT(darray3[0] == 2);
    STANDARD_ASSERT(darray3[1] == 3);
    STANDARD_ASSERT(darray3[2] == 4);
    STANDARD_ASSERT(darray3[3] == 5);
    STANDARD_ASSERT(darray3[4] == 6);
    STANDARD_ASSERT(darray3[5] == 6);
    STANDARD_ASSERT(darray3[6] == 6);
    STANDARD_ASSERT(darray3[7] == 6);
    STANDARD_ASSERT(darray3[8] == 6);
    STANDARD_ASSERT(darray3[9] == 6);
    darray_destroy(darray3);


    // darray_erase_any_matching many at end
    u64* darray4 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);
    u64 ga = 1;
    u64 gb = 2;
    u64 gc = 3;
    u64 gd = 4;
    u64 ge = 5;
    u64 gf = 6;
    u64 gg = 6;
    u64 gh = 6;
    u64 gi = 6;
    u64 gj = 6;
    darray_push(darray4, ga);
    darray_push(darray4, gb);
    darray_push(darray4, gc);
    darray_push(darray4, gd);
    darray_push(darray4, ge);
    darray_push(darray4, gf);
    darray_push(darray4, gg);
    darray_push(darray4, gh);
    darray_push(darray4, gi);
    darray_push(darray4, gj);
    darray_erase_any_matching(darray4, gf);
    STANDARD_ASSERT(darray_length_get(darray4) == 5);
    STANDARD_ASSERT(darray4[0] == 1);
    STANDARD_ASSERT(darray4[1] == 2);
    STANDARD_ASSERT(darray4[2] == 3);
    STANDARD_ASSERT(darray4[3] == 4);
    STANDARD_ASSERT(darray4[4] == 5);
    STANDARD_ASSERT(darray4[5] == 6);
    STANDARD_ASSERT(darray4[6] == 6);
    STANDARD_ASSERT(darray4[7] == 6);
    STANDARD_ASSERT(darray4[8] == 6);
    STANDARD_ASSERT(darray4[9] == 6);
    darray_destroy(darray4);
    memory_pool_destroy(pool1);
}

void ut_darray_zero(void)
{
    memory_pool_st* pool1;
    STANDARD_RESULT res = memory_pool_create("pool1", MiB(1), &pool1);
    (void)res;
    STANDARD_ASSERT(res == 0);

    u64* darray1 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);
    u64 aa = 1;
    u64 ab = 2;
    u64 ac = 3;
    u64 ad = 4;
    u64 ae = 5;
    u64 af = 6;
    u64 ag = 6;
    u64 ah = 6;
    u64 ai = 6;
    u64 aj = 6;
    darray_push(darray1, aa);
    darray_push(darray1, ab);
    darray_push(darray1, ac);
    darray_push(darray1, ad);
    darray_push(darray1, ae);
    darray_push(darray1, af);
    darray_push(darray1, ag);
    darray_push(darray1, ah);
    darray_push(darray1, ai);
    darray_push(darray1, aj);
    darray_zero(darray1);
    STANDARD_ASSERT(darray_length_get(darray1) == 0);
    STANDARD_ASSERT(darray1[0] == 0);
    STANDARD_ASSERT(darray1[1] == 0);
    STANDARD_ASSERT(darray1[2] == 0);
    STANDARD_ASSERT(darray1[3] == 0);
    STANDARD_ASSERT(darray1[4] == 0);
    STANDARD_ASSERT(darray1[5] == 0);
    STANDARD_ASSERT(darray1[6] == 0);
    STANDARD_ASSERT(darray1[7] == 0);
    STANDARD_ASSERT(darray1[8] == 0);
    STANDARD_ASSERT(darray1[9] == 0);

    memory_pool_destroy(pool1);
}

void ut_darray_resize(void)
{
    memory_pool_st* pool1;
    STANDARD_RESULT res = memory_pool_create("pool1", 1, &pool1);
    (void)res;
    STANDARD_ASSERT(res == 0);

    pool_free_zero_list_print(pool1);
    pool_free_not_zero_list_print(pool1);
    pool_allocation_list_print(pool1);

    u64* darray1 = darray_create<u64>(1, pool1, MEMORY_TAG_UNTAGGED);

    // capacity 1 push/pop
    STANDARD_ASSERT(darray_capacity_get(darray1) == 1);
    u64 a = 1234567890;
    darray_push(darray1, a);
    STANDARD_ASSERT(darray_capacity_get(darray1) == 1);
    darray_resize(darray1, 10);
    STANDARD_ASSERT(darray_capacity_get(darray1) == 10);

    // cleanup
    darray_destroy(darray1);
}
