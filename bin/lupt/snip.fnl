(var flag false)

(fn check-show [li kwd cat]
  (if (= li "@@") (set flag false))
  (if flag (print li))
  (if (: li :match (.. "^#+ " cat "_" kwd "[^ ]$")) (set flag true)) )

(fn fsc [cat fname kwd]
 (let [fd (io.open fname)]
   (case fd
     nil (do (print fname "open fail") nil)
     _ (with-open [afd fd]
        (each [li (: afd :lines)]
         (check-show li (: kwd :gsub " " ".- ") cat))))))

(fsc "comp" "_pb_comp" "lu f")
fsc

