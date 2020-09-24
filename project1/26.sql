SELECT name
FROM CatchedPokemon, Pokemon
WHERE pid = Pokemon.id AND nickname LIKE '% %'
ORDER BY name DESC
