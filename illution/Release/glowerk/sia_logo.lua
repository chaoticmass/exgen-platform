
create_sia_logo = function (model) 
--				  w, h, d, x, y, z
--    create_3d_rectangle(model, 64, 64, 64, 0, 0, 0, makecol(128,0,0), 0);

    d = 1;
    u = 0;
    v = 0;
    c1 = makecol(128+10,0,0);
    c2 = makecol(128+30,0,0);
    -- S
    a_x = 9;
    a_y = 42;
    b_x = 38;
    b_y = 42;
    c_x = 20;
    c_y = 17;
    d_x = 19;
    d_y = 21;
    e_x = 32;
    e_y = 39;
    f_x = 10;
    f_y = 39;

    g_x = 51;
    g_y = 16;
    h_x = 22;
    h_y = 16;
    i_x = 40;
    i_y = 40;
    j_x = 41;
    j_y = 36;
    k_x = 28;
    k_y = 19;
    l_x = 50;
    l_y = 19;
    
    -- i
    m_x = 45;
    m_y = 42;
    n_x = 49;
    n_y = 42;
    o_x = 62;
    o_y = 16;
    p_x = 58;
    p_y = 16;
    
    -- a
    q_x = 55;
    q_y = 42;
    r_x = 68;
    r_y = 16;
    s_x = 81;
    s_y = 42;
    t_x = 77;
    t_y = 42;
    u_x = 68;
    u_y = 23;
    v_x = 59;
    v_y = 42;
    
    -- a - b
    model.add_face();
    model.add_face();
    model.set_face_data(0, 0, a_x, a_y, 0, u, v, c1); -- a1
    model.set_face_data(0, 1, b_x, b_y, 0, u, v, c1); -- b1
    model.set_face_data(0, 2, a_x, a_y, d, u, v, c1); -- a2
    model.set_face_data(1, 0, b_x, b_y, 0, u, v, c1); -- b1
    model.set_face_data(1, 1, b_x, b_y, 0, u, v, c1); -- b2
    model.set_face_data(1, 2, a_x, a_y, d, u, v, c1); -- a2

    -- f - e
    model.add_face();
    model.add_face();
    model.set_face_data(2, 0, f_x, f_y, 0, u, v, c1); -- f1
    model.set_face_data(2, 1, e_x, e_y, 0, u, v, c1); -- e1
    model.set_face_data(2, 2, f_x, f_y, d, u, v, c1); -- f2
    model.set_face_data(3, 0, e_x, e_y, 0, u, v, c1); -- e1
    model.set_face_data(3, 1, e_x, e_y, d, u, v, c1); -- e2
    model.set_face_data(3, 2, f_x, f_y, d, u, v, c1); -- f2
    
    -- a - f
    model.add_face();
    model.add_face();
    model.set_face_data(4, 0, a_x, a_y, 0, u, v, c1); -- a1
    model.set_face_data(4, 1, f_x, f_y, 0, u, v, c1); -- f1
    model.set_face_data(4, 2, a_x, a_y, d, u, v, c1); -- a2
    model.set_face_data(5, 0, f_x, f_y, 0, u, v, c1); -- f1
    model.set_face_data(5, 1, f_x, f_y, d, u, v, c1); -- f2
    model.set_face_data(5, 2, a_x, a_y, d, u, v, c1); -- a2
    
    -- e - d
    model.add_face();
    model.add_face();
    model.set_face_data(6, 0, d_x, d_y, 0, u, v, c1); -- d1
    model.set_face_data(6, 1, e_x, e_y, 0, u, v, c1); -- e1
    model.set_face_data(6, 2, d_x, d_y, d, u, v, c1); -- d2
    model.set_face_data(7, 0, e_x, e_y, 0, u, v, c1); -- e1
    model.set_face_data(7, 1, e_x, e_y, d, u, v, c1); -- e2
    model.set_face_data(7, 2, d_x, d_y, d, u, v, c1); -- d2
    
    -- d - c
    model.add_face();
    model.add_face();
    model.set_face_data(8, 0, d_x, d_y, 0, u, v, c1); -- d1
    model.set_face_data(8, 1, c_x, c_y, 0, u, v, c1); -- c1
    model.set_face_data(8, 2, d_x, d_y, d, u, v, c1); -- d2
    model.set_face_data(9, 0, c_x, c_y, 0, u, v, c1); -- c1
    model.set_face_data(9, 1, c_x, c_y, d, u, v, c1); -- c2
    model.set_face_data(9, 2, d_x, d_y, d, u, v, c1); -- d2

    -- c - b
    model.add_face();
    model.add_face();
    model.set_face_data(10, 0, c_x, c_y, 0, u, v, c1); -- c1
    model.set_face_data(10, 1, b_x, b_y, 0, u, v, c1); -- b1
    model.set_face_data(10, 2, c_x, c_y, d, u, v, c1); -- c2
    model.set_face_data(11, 0, b_x, b_y, 0, u, v, c1); -- b1
    model.set_face_data(11, 1, b_x, b_y, d, u, v, c1); -- b2
    model.set_face_data(11, 2, c_x, c_y, d, u, v, c1); -- c2

    -- g - h
    model.add_face();
    model.add_face();
    model.set_face_data(12, 0, g_x, g_y, 0, u, v, c1); -- g1
    model.set_face_data(12, 1, h_x, h_y, 0, u, v, c1); -- h1
    model.set_face_data(12, 2, g_x, g_y, d, u, v, c1); -- g2
    model.set_face_data(13, 0, h_x, h_y, 0, u, v, c1); -- h1
    model.set_face_data(13, 1, h_x, h_y, d, u, v, c1); -- h2
    model.set_face_data(13, 2, g_x, g_y, d, u, v, c1); -- g2

    -- l - k
    model.add_face();
    model.add_face();
    model.set_face_data(14, 0, l_x, l_y, 0, u, v, c1); -- l1
    model.set_face_data(14, 1, k_x, k_y, 0, u, v, c1); -- k1
    model.set_face_data(14, 2, l_x, l_y, d, u, v, c1); -- l2
    model.set_face_data(15, 0, k_x, k_y, 0, u, v, c1); -- k1
    model.set_face_data(15, 1, k_x, k_y, d, u, v, c1); -- k2
    model.set_face_data(15, 2, l_x, l_y, d, u, v, c1); -- l2

    -- g - l
    model.add_face();
    model.add_face();
    model.set_face_data(16, 0, g_x, g_y, 0, u, v, c1); -- g1
    model.set_face_data(16, 1, l_x, l_y, 0, u, v, c1); -- l1
    model.set_face_data(16, 2, g_x, g_y, d, u, v, c1); -- g2
    model.set_face_data(17, 0, l_x, l_y, 0, u, v, c1); -- l1
    model.set_face_data(17, 1, l_x, l_y, d, u, v, c1); -- l2
    model.set_face_data(17, 2, g_x, g_y, d, u, v, c1); -- g2

    -- k - j
    model.add_face();
    model.add_face();
    model.set_face_data(18, 0, k_x, k_y, 0, u, v, c1); -- k1
    model.set_face_data(18, 1, j_x, j_y, 0, u, v, c1); -- j1
    model.set_face_data(18, 2, k_x, k_y, d, u, v, c1); -- k2
    model.set_face_data(19, 0, j_x, j_y, 0, u, v, c1); -- j1
    model.set_face_data(19, 1, j_x, j_y, d, u, v, c1); -- j2
    model.set_face_data(19, 2, k_x, k_y, d, u, v, c1); -- k2

    -- j - i
    model.add_face();
    model.add_face();
    model.set_face_data(20, 0, j_x, j_y, 0, u, v, c1); -- j1
    model.set_face_data(20, 1, i_x, i_y, 0, u, v, c1); -- i1
    model.set_face_data(20, 2, j_x, j_y, d, u, v, c1); -- j2
    model.set_face_data(21, 0, i_x, i_y, 0, u, v, c1); -- i1
    model.set_face_data(21, 1, i_x, i_y, d, u, v, c1); -- i2
    model.set_face_data(21, 2, j_x, j_y, d, u, v, c1); -- j2    

    -- i - h
    model.add_face();
    model.add_face();
    model.set_face_data(22, 0, i_x, i_y, 0, u, v, c1); -- i1
    model.set_face_data(22, 1, h_x, h_y, 0, u, v, c1); -- h1
    model.set_face_data(22, 2, i_x, i_y, d, u, v, c1); -- i2
    model.set_face_data(23, 0, h_x, h_y, 0, u, v, c1); -- h1
    model.set_face_data(23, 1, h_x, h_y, d, u, v, c1); -- h2
    model.set_face_data(23, 2, i_x, i_y, d, u, v, c1); -- i2    

    -- m - n
    model.add_face();
    model.add_face();
    model.set_face_data(24, 0, m_x, m_y, 0, u, v, c1); -- m1
    model.set_face_data(24, 1, n_x, n_y, 0, u, v, c1); -- n1
    model.set_face_data(24, 2, m_x, m_y, d, u, v, c1); -- m2
    model.set_face_data(25, 0, n_x, n_y, 0, u, v, c1); -- n1
    model.set_face_data(25, 1, n_x, n_y, d, u, v, c1); -- n2
    model.set_face_data(25, 2, m_x, m_y, d, u, v, c1); -- m2    

    -- n - o
    model.add_face();
    model.add_face();
    model.set_face_data(26, 0, n_x, n_y, 0, u, v, c1); -- n1
    model.set_face_data(26, 1, o_x, o_y, 0, u, v, c1); -- o1
    model.set_face_data(26, 2, n_x, n_y, d, u, v, c1); -- n2
    model.set_face_data(27, 0, o_x, o_y, 0, u, v, c1); -- o1
    model.set_face_data(27, 1, o_x, o_y, d, u, v, c1); -- o2
    model.set_face_data(27, 2, n_x, n_y, d, u, v, c1); -- n2

    -- o - p
    model.add_face();
    model.add_face();
    model.set_face_data(28, 0, o_x, o_y, 0, u, v, c1); -- o1
    model.set_face_data(28, 1, p_x, p_y, 0, u, v, c1); -- p1
    model.set_face_data(28, 2, o_x, o_y, d, u, v, c1); -- o2
    model.set_face_data(29, 0, p_x, p_y, 0, u, v, c1); -- p1
    model.set_face_data(29, 1, p_x, p_y, d, u, v, c1); -- p2
    model.set_face_data(29, 2, o_x, o_y, d, u, v, c1); -- o2

    -- p - m
    model.add_face();
    model.add_face();
    model.set_face_data(30, 0, p_x, p_y, 0, u, v, c1); -- p1
    model.set_face_data(30, 1, m_x, m_y, 0, u, v, c1); -- m1
    model.set_face_data(30, 2, p_x, p_y, d, u, v, c1); -- p2
    model.set_face_data(31, 0, m_x, m_y, 0, u, v, c1); -- m1
    model.set_face_data(31, 1, m_x, m_y, d, u, v, c1); -- m2
    model.set_face_data(31, 2, p_x, p_y, d, u, v, c1); -- p2

    -- q - r
    model.add_face();
    model.add_face();
    model.set_face_data(32, 0, q_x, q_y, 0, u, v, c1); -- q1
    model.set_face_data(32, 1, r_x, r_y, 0, u, v, c1); -- r1
    model.set_face_data(32, 2, q_x, q_y, d, u, v, c1); -- q2
    model.set_face_data(33, 0, r_x, r_y, 0, u, v, c1); -- r1
    model.set_face_data(33, 1, r_x, r_y, d, u, v, c1); -- r2
    model.set_face_data(33, 2, q_x, q_y, d, u, v, c1); -- q2

    -- r - s
    model.add_face();
    model.add_face();
    model.set_face_data(34, 0, r_x, r_y, 0, u, v, c1); -- r1
    model.set_face_data(34, 1, s_x, s_y, 0, u, v, c1); -- s1
    model.set_face_data(34, 2, r_x, r_y, d, u, v, c1); -- r2
    model.set_face_data(35, 0, s_x, s_y, 0, u, v, c1); -- s1
    model.set_face_data(35, 1, s_x, s_y, d, u, v, c1); -- s2
    model.set_face_data(35, 2, r_x, r_y, d, u, v, c1); -- r2

    -- s - t
    model.add_face();
    model.add_face();
    model.set_face_data(36, 0, s_x, s_y, 0, u, v, c1); -- s1
    model.set_face_data(36, 1, t_x, t_y, 0, u, v, c1); -- t1
    model.set_face_data(36, 2, s_x, s_y, d, u, v, c1); -- s2
    model.set_face_data(37, 0, t_x, t_y, 0, u, v, c1); -- t1
    model.set_face_data(37, 1, t_x, t_y, d, u, v, c1); -- t2
    model.set_face_data(37, 2, s_x, s_y, d, u, v, c1); -- s2

    -- t - u
    model.add_face();
    model.add_face();
    model.set_face_data(38, 0, t_x, t_y, 0, u, v, c1); -- t1
    model.set_face_data(38, 1, u_x, u_y, 0, u, v, c1); -- u1
    model.set_face_data(38, 2, t_x, t_y, d, u, v, c1); -- t2
    model.set_face_data(39, 0, u_x, u_y, 0, u, v, c1); -- u1
    model.set_face_data(39, 1, u_x, u_y, d, u, v, c1); -- u2
    model.set_face_data(39, 2, t_x, t_y, d, u, v, c1); -- t2

    -- u - v
    model.add_face();
    model.add_face();
    model.set_face_data(40, 0, u_x, u_y, 0, u, v, c1); -- u1
    model.set_face_data(40, 1, v_x, v_y, 0, u, v, c1); -- v1
    model.set_face_data(40, 2, u_x, u_y, d, u, v, c1); -- u2
    model.set_face_data(41, 0, v_x, v_y, 0, u, v, c1); -- v1
    model.set_face_data(41, 1, v_x, v_y, d, u, v, c1); -- v2
    model.set_face_data(41, 2, u_x, u_y, d, u, v, c1); -- u2

    -- v - q
    model.add_face();
    model.add_face();
    model.set_face_data(42, 0, v_x, v_y, 0, u, v, c1); -- v1
    model.set_face_data(42, 1, q_x, q_y, 0, u, v, c1); -- q1
    model.set_face_data(42, 2, v_x, v_y, d, u, v, c1); -- v2
    model.set_face_data(43, 0, q_x, q_y, 0, u, v, c1); -- q1
    model.set_face_data(43, 1, q_x, q_y, d, u, v, c1); -- q2
    model.set_face_data(43, 2, v_x, v_y, d, u, v, c1); -- v2

    -- front faces
    -- afb
    model.add_face();
    model.set_face_data(44, 0, a_x, a_y, 0, u, v, c2); -- a
    model.set_face_data(44, 1, f_x, f_y, 0, u, v, c2); -- f
    model.set_face_data(44, 2, b_x, b_y, 0, u, v, c2); -- b
    -- feb
    model.add_face();
    model.set_face_data(45, 0, f_x, f_y, 0, u, v, c2); -- f
    model.set_face_data(45, 1, e_x, e_y, 0, u, v, c2); -- e
    model.set_face_data(45, 2, b_x, b_y, 0, u, v, c2); -- b
    -- ceb
    model.add_face();
    model.set_face_data(46, 0, c_x, c_y, 0, u, v, c2); -- c
    model.set_face_data(46, 1, e_x, e_y, 0, u, v, c2); -- e
    model.set_face_data(46, 2, b_x, b_y, 0, u, v, c2); -- b
    -- dec
    model.add_face();
    model.set_face_data(47, 0, d_x, d_y, 0, u, v, c2); -- d
    model.set_face_data(47, 1, e_x, e_y, 0, u, v, c2); -- e
    model.set_face_data(47, 2, c_x, c_y, 0, u, v, c2); -- c

    -- lkg
    model.add_face();
    model.set_face_data(48, 0, l_x, l_y, 0, u, v, c2); -- l
    model.set_face_data(48, 1, k_x, k_y, 0, u, v, c2); -- k
    model.set_face_data(48, 2, g_x, g_y, 0, u, v, c2); -- g
    -- hkg
    model.add_face();
    model.set_face_data(49, 0, h_x, h_y, 0, u, v, c2); -- h
    model.set_face_data(49, 1, k_x, k_y, 0, u, v, c2); -- k
    model.set_face_data(49, 2, g_x, g_y, 0, u, v, c2); -- g
    -- hik
    model.add_face();
    model.set_face_data(50, 0, h_x, h_y, 0, u, v, c2); -- h
    model.set_face_data(50, 1, i_x, i_y, 0, u, v, c2); -- i
    model.set_face_data(50, 2, k_x, k_y, 0, u, v, c2); -- k
    -- kij
    model.add_face();
    model.set_face_data(51, 0, k_x, k_y, 0, u, v, c2); -- k
    model.set_face_data(51, 1, i_x, i_y, 0, u, v, c2); -- i
    model.set_face_data(51, 2, j_x, j_y, 0, u, v, c2); -- j
    -- mpn
    model.add_face();
    model.set_face_data(52, 0, m_x, m_y, 0, u, v, c2); -- m
    model.set_face_data(52, 1, p_x, p_y, 0, u, v, c2); -- p
    model.set_face_data(52, 2, n_x, n_y, 0, u, v, c2); -- n
    -- nop
    model.add_face();
    model.set_face_data(53, 0, n_x, n_y, 0, u, v, c2); -- n
    model.set_face_data(53, 1, o_x, o_y, 0, u, v, c2); -- o
    model.set_face_data(53, 2, p_x, p_y, 0, u, v, c2); -- p
    -- quv
    model.add_face();
    model.set_face_data(54, 0, q_x, q_y, 0, u, v, c2); -- q
    model.set_face_data(54, 1, u_x, u_y, 0, u, v, c2); -- u
    model.set_face_data(54, 2, v_x, v_y, 0, u, v, c2); -- v
    -- qur
    model.add_face();
    model.set_face_data(55, 0, q_x, q_y, 0, u, v, c2); -- q
    model.set_face_data(55, 1, u_x, u_y, 0, u, v, c2); -- u
    model.set_face_data(55, 2, r_x, r_y, 0, u, v, c2); -- r
    -- rus
    model.add_face();
    model.set_face_data(56, 0, r_x, r_y, 0, u, v, c2); -- r
    model.set_face_data(56, 1, u_x, u_y, 0, u, v, c2); -- u
    model.set_face_data(56, 2, s_x, s_y, 0, u, v, c2); -- s
    -- sut
    model.add_face();
    model.set_face_data(57, 0, s_x, s_y, 0, u, v, c2); -- s
    model.set_face_data(57, 1, u_x, u_y, 0, u, v, c2); -- u
    model.set_face_data(57, 2, t_x, t_y, 0, u, v, c2); -- t
    
end




create_sia_logo_i = function (model, mode) 
--				  w, h, d, x, y, z
--    create_3d_rectangle(model, 64, 64, 64, 0, 0, 0, makecol(128,0,0), 0);

    d = 1;
    u = 0;
    v = 0;
    c1 = makecol(128+10,0,0);
    c2 = makecol(128+30,0,0);

    origin_x = -53;
    origin_y = -30;

    
    -- i
    m_x = origin_x + 45;
    o_x = origin_x + 62;
    n_x = origin_x + 49;
    p_x = origin_x + 58;
    
    m_y = origin_y + 42;
    n_y = origin_y + 42;
    o_y = origin_y + 16;
    p_y = origin_y + 16;
    
    
if (mode == 1) then
    fc = model.face_count;
    -- m - n
    model.add_face();
    model.add_face();
    model.set_face_data(fc+0, 0, m_x, m_y, 0, u, v, c1); -- m1
    model.set_face_data(fc+0, 1, n_x, n_y, 0, u, v, c1); -- n1
    model.set_face_data(fc+0, 2, m_x, m_y, d, u, v, c1); -- m2
    model.set_face_data(fc+1, 0, n_x, n_y, 0, u, v, c1); -- n1
    model.set_face_data(fc+1, 1, n_x, n_y, d, u, v, c1); -- n2
    model.set_face_data(fc+1, 2, m_x, m_y, d, u, v, c1); -- m2    

    -- n - o
    model.add_face();
    model.add_face();
    model.set_face_data(fc+2, 0, n_x, n_y, 0, u, v, c1); -- n1
    model.set_face_data(fc+2, 1, o_x, o_y, 0, u, v, c1); -- o1
    model.set_face_data(fc+2, 2, n_x, n_y, d, u, v, c1); -- n2
    model.set_face_data(fc+3, 0, o_x, o_y, 0, u, v, c1); -- o1
    model.set_face_data(fc+3, 1, o_x, o_y, d, u, v, c1); -- o2
    model.set_face_data(fc+3, 2, n_x, n_y, d, u, v, c1); -- n2

    -- o - p
    model.add_face();
    model.add_face();
    model.set_face_data(fc+4, 0, o_x, o_y, 0, u, v, c1); -- o1
    model.set_face_data(fc+4, 1, p_x, p_y, 0, u, v, c1); -- p1
    model.set_face_data(fc+4, 2, o_x, o_y, d, u, v, c1); -- o2
    model.set_face_data(fc+5, 0, p_x, p_y, 0, u, v, c1); -- p1
    model.set_face_data(fc+5, 1, p_x, p_y, d, u, v, c1); -- p2
    model.set_face_data(fc+5, 2, o_x, o_y, d, u, v, c1); -- o2

    -- p - m
    model.add_face();
    model.add_face();
    model.set_face_data(fc+6, 0, p_x, p_y, 0, u, v, c1); -- p1
    model.set_face_data(fc+6, 1, m_x, m_y, 0, u, v, c1); -- m1
    model.set_face_data(fc+6, 2, p_x, p_y, d, u, v, c1); -- p2
    model.set_face_data(fc+7, 0, m_x, m_y, 0, u, v, c1); -- m1
    model.set_face_data(fc+7, 1, m_x, m_y, d, u, v, c1); -- m2
    model.set_face_data(fc+7, 2, p_x, p_y, d, u, v, c1); -- p2
end
if (mode == 0) then
    fc = model.face_count - 8;
    -- front faces
    -- mpn
    model.add_face();
    model.set_face_data(fc+8, 0, m_x, m_y, 0, u, v, c2); -- m
    model.set_face_data(fc+8, 1, p_x, p_y, 0, u, v, c2); -- p
    model.set_face_data(fc+8, 2, n_x, n_y, 0, u, v, c2); -- n
    -- nop
    model.add_face();
    model.set_face_data(fc+9, 0, n_x, n_y, 0, u, v, c2); -- n
    model.set_face_data(fc+9, 1, o_x, o_y, 0, u, v, c2); -- o
    model.set_face_data(fc+9, 2, p_x, p_y, 0, u, v, c2); -- p
end
end



create_sia_logo_s = function (model, mode) 
--				  w, h, d, x, y, z
--    create_3d_rectangle(model, 64, 64, 64, 0, 0, 0, makecol(128,0,0), 0);

    d = 1;
    u = 0;
    v = 0;
    c1 = makecol(128+10,0,0);
    c2 = makecol(128+30,0,0);
    c3 = makecol(0,128,0);

    origin_x = -20;
    origin_y = -32;

    -- a
--[[    q_y = origin_y + 42;
    r_y = origin_y + 16;
    s_y = origin_y + 42;
    t_y = origin_y + 42;
    u_y = origin_y + 23;
    v_y = origin_y + 42;
    
    q_x = origin_x + 55;
    r_x = origin_x + 68;
    s_x = origin_x + 81;
    t_x = origin_x + 77;
    u_x = origin_x + 68;
    v_x = origin_x + 59; ]]
    
    q_x = origin_x + 8.5;
    r_x = origin_x + 38;
    s_x = origin_x + 21;
    t_x = origin_x + 19;
    u_x = origin_x + 32;
    v_x = origin_x + 10;

    
    q_y = origin_y + 42;
    r_y = origin_y + 42;
    s_y = origin_y + 17;
    t_y = origin_y + 21;
    u_y = origin_y + 38;
    v_y = origin_y + 38;
    
    

if (mode == 1) then
    fc = model.face_count;
    -- q - r
    -- a - b
    model.add_face();
    model.add_face();
    model.set_face_data(fc+0, 0, q_x, q_y, 0, u, v, c1); -- q1
    model.set_face_data(fc+0, 1, r_x, r_y, 0, u, v, c1); -- r1
    model.set_face_data(fc+0, 2, q_x, q_y, d, u, v, c1); -- q2
    model.set_face_data(fc+1, 0, r_x, r_y, 0, u, v, c1); -- r1
    model.set_face_data(fc+1, 1, r_x, r_y, d, u, v, c1); -- r2
    model.set_face_data(fc+1, 2, q_x, q_y, d, u, v, c1); -- q2

    -- r - s
    -- b - c
    model.add_face();
    model.add_face();
    model.set_face_data(fc+2, 0, r_x, r_y, 0, u, v, c1); -- r1
    model.set_face_data(fc+2, 1, s_x, s_y, 0, u, v, c1); -- s1
    model.set_face_data(fc+2, 2, r_x, r_y, d, u, v, c1); -- r2
    model.set_face_data(fc+3, 0, s_x, s_y, 0, u, v, c1); -- s1
    model.set_face_data(fc+3, 1, s_x, s_y, d, u, v, c1); -- s2
    model.set_face_data(fc+3, 2, r_x, r_y, d, u, v, c1); -- r2

    -- s - t
    -- c - d
    model.add_face();
    model.add_face();
    model.set_face_data(fc+4, 0, s_x, s_y, 0, u, v, c1); -- s1
    model.set_face_data(fc+4, 1, t_x, t_y, 0, u, v, c1); -- t1
    model.set_face_data(fc+4, 2, s_x, s_y, d, u, v, c1); -- s2
    model.set_face_data(fc+5, 0, t_x, t_y, 0, u, v, c1); -- t1
    model.set_face_data(fc+5, 1, t_x, t_y, d, u, v, c1); -- t2
    model.set_face_data(fc+5, 2, s_x, s_y, d, u, v, c1); -- s2

    -- t - u
    -- d - e
    model.add_face();
    model.add_face();
    model.set_face_data(fc+6, 0, t_x, t_y, 0, u, v, c1); -- t1
    model.set_face_data(fc+6, 1, u_x, u_y, 0, u, v, c1); -- u1
    model.set_face_data(fc+6, 2, t_x, t_y, d, u, v, c1); -- t2
    model.set_face_data(fc+7, 0, u_x, u_y, 0, u, v, c1); -- u1
    model.set_face_data(fc+7, 1, u_x, u_y, d, u, v, c1); -- u2
    model.set_face_data(fc+7, 2, t_x, t_y, d, u, v, c1); -- t2

    -- u - v
    -- e - f
    model.add_face();
    model.add_face();
    model.set_face_data(fc+8, 0, u_x, u_y, 0, u, v, c1); -- u1
    model.set_face_data(fc+8, 1, v_x, v_y, 0, u, v, c1); -- v1
    model.set_face_data(fc+8, 2, u_x, u_y, d, u, v, c1); -- u2
    model.set_face_data(fc+9, 0, v_x, v_y, 0, u, v, c1); -- v1
    model.set_face_data(fc+9, 1, v_x, v_y, d, u, v, c1); -- v2
    model.set_face_data(fc+9, 2, u_x, u_y, d, u, v, c1); -- u2

    -- v - q
    -- f - a
    model.add_face();
    model.add_face();
    model.set_face_data(fc+10, 0, v_x, v_y, 0, u, v, c1); -- v1
    model.set_face_data(fc+10, 1, q_x, q_y, 0, u, v, c1); -- q1
    model.set_face_data(fc+10, 2, v_x, v_y, d, u, v, c1); -- v2
    model.set_face_data(fc+11, 0, q_x, q_y, 0, u, v, c1); -- q1
    model.set_face_data(fc+11, 1, q_x, q_y, d, u, v, c1); -- q2
    model.set_face_data(fc+11, 2, v_x, v_y, d, u, v, c1); -- v2
end
if (mode == 0) then
    fc = model.face_count - 12;
    -- quv
    model.add_face();
    model.set_face_data(fc+12, 0, q_x, q_y, 0, u, v, c2); -- q
    model.set_face_data(fc+12, 1, u_x, u_y, 0, u, v, c2); -- u
    model.set_face_data(fc+12, 2, v_x, v_y, 0, u, v, c2); -- v
    -- qur
    model.add_face();
    model.set_face_data(fc+13, 0, q_x, q_y, 0, u, v, c2); -- q
    model.set_face_data(fc+13, 1, u_x, u_y, 0, u, v, c2); -- u
    model.set_face_data(fc+13, 2, r_x, r_y, 0, u, v, c2); -- r
    -- rus
    model.add_face();
    model.set_face_data(fc+14, 0, r_x, r_y, 0, u, v, c2); -- r
    model.set_face_data(fc+14, 1, u_x, u_y, 0, u, v, c2); -- u
    model.set_face_data(fc+14, 2, s_x, s_y, 0, u, v, c2); -- s
    -- sut
    model.add_face();
    model.set_face_data(fc+15, 0, s_x, s_y, 0, u, v, c2); -- s
    model.set_face_data(fc+15, 1, u_x, u_y, 0, u, v, c2); -- u
    model.set_face_data(fc+15, 2, t_x, t_y, 0, u, v, c2); -- t
end
    
end

create_sia_logo_a = function (model, mode) 
--				  w, h, d, x, y, z
--    create_3d_rectangle(model, 64, 64, 64, 0, 0, 0, makecol(128,0,0), 0);

    d = 1;
    u = 0;
    v = 0;
    c1 = makecol(128+10,0,0);
    c2 = makecol(128+30,0,0);
    c3 = makecol(0,128,0);

    origin_x = -68;
    origin_y = -30;

    -- a
    q_y = origin_y + 42;
    r_y = origin_y + 16;
    s_y = origin_y + 42;
    t_y = origin_y + 42;
    u_y = origin_y + 23;
    v_y = origin_y + 42;
    
    q_x = origin_x + 55;
    r_x = origin_x + 68;
    s_x = origin_x + 81;
    t_x = origin_x + 77;
    u_x = origin_x + 68;
    v_x = origin_x + 59; 
    
    -- q - r
    -- a - b
    
if (mode == 1) then
    fc = model.face_count;
    model.add_face();
    model.add_face();
    model.set_face_data(fc+0, 0, q_x, q_y, 0, u, v, c1); -- q1
    model.set_face_data(fc+0, 1, r_x, r_y, 0, u, v, c1); -- r1
    model.set_face_data(fc+0, 2, q_x, q_y, d, u, v, c1); -- q2
    model.set_face_data(fc+1, 0, r_x, r_y, 0, u, v, c1); -- r1
    model.set_face_data(fc+1, 1, r_x, r_y, d, u, v, c1); -- r2
    model.set_face_data(fc+1, 2, q_x, q_y, d, u, v, c1); -- q2

    -- r - s
    -- b - c
    model.add_face();
    model.add_face();
    model.set_face_data(fc+2, 0, r_x, r_y, 0, u, v, c1); -- r1
    model.set_face_data(fc+2, 1, s_x, s_y, 0, u, v, c1); -- s1
    model.set_face_data(fc+2, 2, r_x, r_y, d, u, v, c1); -- r2
    model.set_face_data(fc+3, 0, s_x, s_y, 0, u, v, c1); -- s1
    model.set_face_data(fc+3, 1, s_x, s_y, d, u, v, c1); -- s2
    model.set_face_data(fc+3, 2, r_x, r_y, d, u, v, c1); -- r2

    -- s - t
    -- c - d
    model.add_face();
    model.add_face();
    model.set_face_data(fc+4, 0, s_x, s_y, 0, u, v, c1); -- s1
    model.set_face_data(fc+4, 1, t_x, t_y, 0, u, v, c1); -- t1
    model.set_face_data(fc+4, 2, s_x, s_y, d, u, v, c1); -- s2
    model.set_face_data(fc+5, 0, t_x, t_y, 0, u, v, c1); -- t1
    model.set_face_data(fc+5, 1, t_x, t_y, d, u, v, c1); -- t2
    model.set_face_data(fc+5, 2, s_x, s_y, d, u, v, c1); -- s2

    -- t - u
    -- d - e
    model.add_face();
    model.add_face();
    model.set_face_data(fc+6, 0, t_x, t_y, 0, u, v, c1); -- t1
    model.set_face_data(fc+6, 1, u_x, u_y, 0, u, v, c1); -- u1
    model.set_face_data(fc+6, 2, t_x, t_y, d, u, v, c1); -- t2
    model.set_face_data(fc+7, 0, u_x, u_y, 0, u, v, c1); -- u1
    model.set_face_data(fc+7, 1, u_x, u_y, d, u, v, c1); -- u2
    model.set_face_data(fc+7, 2, t_x, t_y, d, u, v, c1); -- t2

    -- u - v
    -- e - f
    model.add_face();
    model.add_face();
    model.set_face_data(fc+8, 0, u_x, u_y, 0, u, v, c1); -- u1
    model.set_face_data(fc+8, 1, v_x, v_y, 0, u, v, c1); -- v1
    model.set_face_data(fc+8, 2, u_x, u_y, d, u, v, c1); -- u2
    model.set_face_data(fc+9, 0, v_x, v_y, 0, u, v, c1); -- v1
    model.set_face_data(fc+9, 1, v_x, v_y, d, u, v, c1); -- v2
    model.set_face_data(fc+9, 2, u_x, u_y, d, u, v, c1); -- u2

    -- v - q
    -- f - a
    model.add_face();
    model.add_face();
    model.set_face_data(fc+10, 0, v_x, v_y, 0, u, v, c1); -- v1
    model.set_face_data(fc+10, 1, q_x, q_y, 0, u, v, c1); -- q1
    model.set_face_data(fc+10, 2, v_x, v_y, d, u, v, c1); -- v2
    model.set_face_data(fc+11, 0, q_x, q_y, 0, u, v, c1); -- q1
    model.set_face_data(fc+11, 1, q_x, q_y, d, u, v, c1); -- q2
    model.set_face_data(fc+11, 2, v_x, v_y, d, u, v, c1); -- v2
end
if (mode == 0) then
    fc = model.face_count - 12;
    -- quv
    model.add_face();
    model.set_face_data(fc+12, 0, q_x, q_y, 0, u, v, c2); -- q
    model.set_face_data(fc+12, 1, u_x, u_y, 0, u, v, c2); -- u
    model.set_face_data(fc+12, 2, v_x, v_y, 0, u, v, c2); -- v
    -- qur
    model.add_face();
    model.set_face_data(fc+13, 0, q_x, q_y, 0, u, v, c2); -- q
    model.set_face_data(fc+13, 1, u_x, u_y, 0, u, v, c2); -- u
    model.set_face_data(fc+13, 2, r_x, r_y, 0, u, v, c2); -- r
    -- rus
    model.add_face();
    model.set_face_data(fc+14, 0, r_x, r_y, 0, u, v, c2); -- r
    model.set_face_data(fc+14, 1, u_x, u_y, 0, u, v, c2); -- u
    model.set_face_data(fc+14, 2, s_x, s_y, 0, u, v, c2); -- s
    -- sut
    model.add_face();
    model.set_face_data(fc+15, 0, s_x, s_y, 0, u, v, c2); -- s
    model.set_face_data(fc+15, 1, u_x, u_y, 0, u, v, c2); -- u
    model.set_face_data(fc+15, 2, t_x, t_y, 0, u, v, c2); -- t
end

    
end


tikz_logo = function () 
    cords = {
	a = {x = 9,  y = 42},
	b = {x = 38, y = 42},
	c = {x = 20, y = 17},
	d = {x = 19, y = 21},
	e = {x = 32, y = 39},
	f = {x = 10, y = 39},
	g = {x = 51, y = 16},
	h = {x = 22, y = 16},
	i = {x = 40, y = 40},
	j = {x = 41, y = 36},
	k = {x = 28, y = 19},
	l = {x = 50, y = 19},
	m = {x = 45, y = 42},
	n = {x = 49, y = 42},
	o = {x = 62, y = 16},
	p = {x = 58, y = 16},
	q = {x = 55, y = 42},
	r = {x = 68, y = 16},
	s = {x = 81, y = 42},
	t = {x = 77, y = 42},
	u = {x = 68, y = 23},
	v = {x = 59, y = 42}
    };

    for key, value in pairs(cords) do
	echo("("..cords[key].x..","..cords[key].y..") -- ");
    end
end