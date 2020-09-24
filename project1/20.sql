SELECT name, COUNT(*) AS CatchPokemonNUM
FROM CatchedPokemon, Trainer
WHERE owner_id = Trainer.id AND Hometown = 'Sangnok City'
GROUP BY Trainer.id 
ORDER BY COUNT(*) ASC
