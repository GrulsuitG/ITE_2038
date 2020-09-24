SELECT Pokemon.name
FROM Pokemon
WHERE id IN (SELECT pid
             FROM CatchedPokemon, Trainer
             WHERE owner_id = Trainer.id AND hometown = 'Sangnok City')
      AND id IN (SELECT pid
                 FROM CatchedPokemon, Trainer
                 WHERE owner_id = Trainer.id AND hometown = 'Brown City')
ORDER BY name ASC
