Etudiant :
CHAPUT Rémy n°11409724
GUEUX Grégory n°11407831

Nous avons créé une fonction expr2Aut, utilisée de manière récursive pour créer
un automate à partir d'une expression rationnelle. La fonction ExpressionRationnelle2Automate
crée un arbre de syntaxe abstraite à partir d'une chaîne de caractères, puis appelle expr2Aut.
expr2Aut crée un automate selon son opérateur et s'appelle récursivement si l'opérateur est |, . ou *

Afin de se servir des automates générés par aut2expr puis expr2aut, nous avons changé la fonction operator<<,
de manière à ce qu'elle écrive dans le fichier txt les informations nécessaires à la création d'un automate (comme les
exemples fournis).
