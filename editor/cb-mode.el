;;; cb-mode.el --- Major mode for ^! -*- lexical-binding: t -*-

(defconst cb-mode-syntax-table
  (let ((st (make-syntax-table)))
    (modify-syntax-entry ?\[ "(]" st)
    (modify-syntax-entry ?\] ")[" st)
    (modify-syntax-entry ?\( "<)nb" st)
    (modify-syntax-entry ?\) ">(nb" st)
    st)
  "Syntax table for cb-mode")

(defconst cb-instructions
  '("^" "!" "+" "-" "," "." "[" "]" ":" "*" "%" "@")
  "^! reserved instruction characters")

(defconst cb-highlighting
  `((,(regexp-opt cb-instructions t) . font-lock-keyword-face))
  "Instruction highlighting regex for ^!")

;;;###autoload
(define-derived-mode cb-mode prog-mode "^!"
  "Major mode for ^!"
  :syntax-table cb-mode-syntax-table
  (setq font-lock-defaults '(cb-highlighting)))


;;;###autoload
(add-to-list 'auto-mode-alist '("\\.cb\\'" . cb-mode))

;(provide 'cb-mode)
;;; cb-mode.el ends here
