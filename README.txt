Etudiant :
CHAPUT Rémy n°11409724
GUEUX Grégory n°11407831

Nous avons créé une fonction expr2Aut, utilisée de manière récursive pour créer
un automate à partir d'une expression rationnelle. La fonction ExpressionRationnelle2Automate
crée un arbre de syntaxe abstraite à partir d'une chaîne de caractères, puis appelle expr2Aut.
expr2Aut crée un automate selon son opérateur et s'appelle récursivement si l'opérateur est |, . ou *
