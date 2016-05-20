The aim of the project is to enable the automatic insertion of numbers, figures etc. into latex documents without the need for manual intervention. This is acheived by adding the following syntax to .tex files:


```latex
\documentclass{article}
\begin{document}
Given $x = << value >>$, we know that $x^2 = << value**2 >>$.
\end{document}

```




This work is based on a blog post written by Igor: [[https://babushk.in/posts/diy-automated-papers.html]]
