The aim of the project is to enable the automatic insertion of numbers, figures etc. into LaTeX documents without the need for manual intervention. This is acheived by adding the following syntax to .tex files:


```latex
\documentclass{article}
\begin{document}
Given $x = << value >>$, we know that $x^2 = << value**2 >>$.
\end{document}

```

Papermake uses (Jinja2)[http://jinja.pocoo.org/] to insert values from Python (specifically using pickle files) using a custom syntax:

| syntax | what for |
|--------|----------|
| `#<` | start jinja block |
| `#>` | end jinja block |
| `<<` | start python variable |
| `>>` | end python variable |
| `#=` | start jinja comment block |
| `#=` | end jinja comment block |

(The normal Jinja2 syntax uses `{{ variable_name }}` which clashes with LaTeX's own syntax)

When this extra syntax is used in LaTeX files, Papermake reads the file, and creates a new file with the python varibles inserted into the text. If we had `value = 5`, then the LaTeX block above would become

```latex
\documentclass{article}
\begin{document}
Given $x = 5$, we know that $x^2 = 25$.
\end{document}

```

Jinja2 also has basic loop and conditional statements, which an be found at http://jinja.pocoo.org.

This work is based on a blog post written by Igor: https://babushk.in/posts/diy-automated-papers.html
