SELECT name
FROM CatchedPokemon, Trainer
WHERE owner_id = Trainer.id
GROUP BY owner_id
HAVING count(*)>2
ORDER BY count(*) DESC

