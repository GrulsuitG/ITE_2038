SELECT Trainer.name , AVG(level)
FROM Trainer, CatchedPokemon, Pokemon
WHERE Trainer.id = owner_id AND pid = Pokemon.id AND
      type IN ('Normal', 'Electric')
GROUP BY Trainer.id
ORDER BY AVG(level) ASC
