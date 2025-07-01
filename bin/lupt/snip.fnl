(var flag false)
(fn show-range [li mat]
  (if (= li "@@") (set flag false))
  (if flag (print li))
  (if (: li :match mat) (set flag true)))

(fn show-title [li mat]
  (if (: li :match mat) (print (: li :slice 8))))

(fn onef [cat fname kwd]
"find snip/comp(category) with kwd in fname"
 (let [fd (io.open fname) pkwd (: kwd :gsub " " ".- ") r_mat (.. "^#+ " cat "_" pkwd "[^ ]*$") t_mat (.. "^# " cat "_")]
  (case fd
    nil nil
    _ (with-open [afd fd]
       (each [li (: afd :lines)]
        (if (= "" kwd)
         (show-title li t_mat)
         (show-range li r_mat)))
      1))))

(fn fsc [cat fname kwd]
"main entry for snip or comp"
  (let [tkwd (: kwd :trim)]
    (onef cat fname tkwd)
    (for [i 1 9 &until (onef cat (.. fname i) tkwd)]
    1)))

;(fsc "snip" "_pb_snip" "sh va ")
fsc

